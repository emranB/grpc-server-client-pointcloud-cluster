#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <grpcpp/grpcpp.h>
#include <fmt/core.h>  // Include fmt library
#include "pointcloud.pb.h"
#include "pointcloud.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReaderWriter;
using grpc::Status;
using pointcloud::NLPoint;
using pointcloud::NLChunkRequest;
using pointcloud::NLClusterResponse;
using pointcloud::NLPointCloudService;

class PointCloudClient {
public:
    PointCloudClient(std::shared_ptr<Channel> channel)
        : mStub(NLPointCloudService::NewStub(channel)) {}

    void streamPointCloud() {
        ClientContext context;
        auto stream = mStub->Cluster(&context);

        // Fetch file paths from environment variables
        const char* inputFile = std::getenv("CLIENT_INPUT_FILE");
        const char* outputFile = std::getenv("CLIENT_OUTPUT_FILE");
        const char* labelsFile = std::getenv("CLIENT_LABELS_FILE");

        if (!inputFile || !outputFile || !labelsFile) {
            fmt::print(stderr, "Error: Failed to get file paths from environment.\n");
            return;
        }

        // Open input file for reading point cloud data
        std::ifstream infile(inputFile);
        if (!infile.is_open()) {
            fmt::print(stderr, "Error: Failed to open input file: {}\n", inputFile);
            return;
        }

        // Open output file for writing point labels
        std::ofstream outputLabelsFile(outputFile);
        if (!outputLabelsFile.is_open()) {
            fmt::print(stderr, "Error: Failed to open output labels file: {}\n", outputFile);
            return;
        }

        // Parse the labels file for expected results
        std::ifstream labelFile(labelsFile);
        int expectedDura = 0, expectedCortical = 0, expectedUnknown = 0;
        std::string line;
        while (std::getline(labelFile, line)) {
            if (line.find("0") != std::string::npos) {
                expectedUnknown++;
            } else if (line.find("1") != std::string::npos) {
                expectedCortical++;
            } else if (line.find("2") != std::string::npos) {
                expectedDura++;
            }
        }
        labelFile.close();

        // Calculate percentages for expected results
        int totalExpected = expectedDura + expectedCortical + expectedUnknown;
        float duraPercent = (float)expectedDura / totalExpected * 100;
        float corticalPercent = (float)expectedCortical / totalExpected * 100;
        float unknownPercent = (float)expectedUnknown / totalExpected * 100;

        // Write the expected results with percentages to the output file
        std::ofstream expectedFile("/app/results/oct-2-small-output.expected");
        expectedFile << "Expected Results, based on " << labelsFile << ":\n";
        expectedFile << "DURA count: " << expectedDura << " (" << duraPercent << "%)\n";
        expectedFile << "CORTICAL_SURFACE count: " << expectedCortical << " (" << corticalPercent << "%)\n";
        expectedFile << "UNKNOWN count: " << expectedUnknown << " (" << unknownPercent << "%)\n";
        expectedFile.close();

        // Process point cloud data and send chunks to the server
        NLChunkRequest chunk;
        int pointId = 1;
        while (std::getline(infile, line)) {
            if (line.empty() || line[0] == '#') continue;  // Skip empty or comment lines

            NLPoint* point = chunk.add_points();
            float x, y, z;
            sscanf(line.c_str(), "%f %f %f", &x, &y, &z);
            point->set_x(x);
            point->set_y(y);
            point->set_z(z);
            point->set_id(pointId++);

            // Send the chunk if it reaches the chunk size limit
            if (chunk.points_size() >= 100) {
                stream->Write(chunk);
                chunk.Clear();  // Clear the chunk for the next set of points
            }
        }

        // Send the remaining points if any
        if (chunk.points_size() > 0) {
            stream->Write(chunk);
        }

        // Close the input file stream
        infile.close();
        stream->WritesDone();  // Mark the end of writing

        // Read the server responses and write to labels file
        NLClusterResponse response;
        int duraCount = 0, corticalCount = 0, unknownCount = 0;
        int totalPointsReceived = 0, totalPointsProcessed = 0, totalPointsDiscarded = 0;

        while (stream->Read(&response)) {
            fmt::print("Received Point ID: {}, Label: {}\n", response.point_id(), response.label());
            outputLabelsFile << "Point ID: " << response.point_id() << ", Label: " << response.label() << std::endl;

            // Update counts
            if (response.label() == 2) duraCount++;
            if (response.label() == 1) corticalCount++;
            if (response.label() == 0) unknownCount++;

            totalPointsReceived++;
            if (response.label() == 0) totalPointsDiscarded++;
            else totalPointsProcessed++;
        }

        // Write the summary to summaryFile
        std::ofstream summaryFile("/app/results/oct-2-small-output.summary");
        int totalPoints = duraCount + corticalCount + unknownCount;
        summaryFile << "Summary of clustering:\n";
        summaryFile << "Total points received: " << totalPointsReceived << "\n";
        summaryFile << "Total points processed: " << totalPointsProcessed << "\n";
        summaryFile << "Total points discarded: " << totalPointsDiscarded << "\n";
        summaryFile << "DURA count: " << duraCount << " (" << (duraCount / (float)totalPoints) * 100 << "%)\n";
        summaryFile << "CORTICAL_SURFACE count: " << corticalCount << " (" << (corticalCount / (float)totalPoints) * 100 << "%)\n";
        summaryFile << "UNKNOWN count: " << unknownCount << " (" << (unknownCount / (float)totalPoints) * 100 << "%)\n";
        summaryFile.close();

        // Finish the stream and check for errors
        Status status = stream->Finish();
        if (!status.ok()) {
            fmt::print(stderr, "Error: gRPC stream failed: {}\n", status.error_message());
        }

        // Close the output file stream
        outputLabelsFile.close();
    }

private:
    std::unique_ptr<NLPointCloudService::Stub> mStub;  // Corrected member name with camel case and m_ prefix
};

int main(int argc, char** argv) {
    // Fetch environment variables (ensure names match the Docker Compose environment)
    const char* host = std::getenv("CLIENT_REMOTE_HOST");
    const char* port = std::getenv("CLIENT_REMOTE_PORT");

    if (!host || !port) {
        fmt::print(stderr, "Error: Server host or port not specified in environment.\n");
        return 1;
    }

    // Build the server address
    std::string target = std::string(host) + ":" + std::string(port);

    // Create the client
    PointCloudClient client(grpc::CreateChannel(target, grpc::InsecureChannelCredentials()));

    fmt::print("Streaming point cloud data to server: {}\n", target);
    client.streamPointCloud();  // Stream the point cloud data

    return 0;
}

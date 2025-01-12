#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <grpcpp/grpcpp.h>
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
        : stub_(NLPointCloudService::NewStub(channel)) {}

    void StreamPointCloud() {
        ClientContext context;
        auto stream = stub_->Cluster(&context);

        // Fetch file paths from environment variables
        const char* inputFile = std::getenv("CLIENT_INPUT_FILE");
        if (!inputFile) {
            std::cerr << "Error: Failed to get input file path from environment." << std::endl;
            return;
        }

        const char* outputFile = std::getenv("CLIENT_OUTPUT_FILE");
        if (!outputFile) {
            std::cerr << "Error: Failed to get output file path from environment." << std::endl;
            return;
        }

        // Open input file for reading point cloud data
        std::ifstream infile(inputFile);
        if (!infile.is_open()) {
            std::cerr << "Error: Failed to open input file: " << inputFile << std::endl;
            return;
        }

        std::ofstream outfile(outputFile);  // Open output file for writing results
        if (!outfile.is_open()) {
            std::cerr << "Error: Failed to open output file: " << outputFile << std::endl;
            return;
        }

        NLChunkRequest chunk;
        std::string line;
        int point_id = 1;

        // Read the input PCD file and send chunks to the server
        while (std::getline(infile, line)) {
            if (line.empty() || line[0] == '#') continue;  // Skip empty or comment lines

            NLPoint* point = chunk.add_points();
            float x, y, z;
            sscanf(line.c_str(), "%f %f %f", &x, &y, &z);
            point->set_x(x);
            point->set_y(y);
            point->set_z(z);
            point->set_id(point_id++);

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

        // Read the server responses
        NLClusterResponse response;
        while (stream->Read(&response)) {
            // Write the results to the output file
            outfile << "Point ID: " << response.point_id() << ", Label: " << response.label() << std::endl;
        }

        // Finish the stream and check for errors
        Status status = stream->Finish();
        if (!status.ok()) {
            std::cerr << "Error: gRPC stream failed: " << status.error_message() << std::endl;
        }

        // Close the output file stream
        outfile.close();
    }

private:
    std::unique_ptr<NLPointCloudService::Stub> stub_;
};

int main(int argc, char** argv) {
    // Fetch the server details from environment variables
    const char* serverHost = std::getenv("CLIENT_REMOTE_HOST");
    const char* serverPort = std::getenv("CLIENT_REMOTE_PORT");

    if (!serverHost || !serverPort) {
        std::cerr << "Error: Server host or port not specified in environment." << std::endl;
        return 1;
    }

    // Build the server address
    std::string target = std::string(serverHost) + ":" + std::string(serverPort);

    // Create the client
    PointCloudClient client(grpc::CreateChannel(target, grpc::InsecureChannelCredentials()));

    std::cout << "Streaming point cloud data to server: " << target << "...\n";
    client.StreamPointCloud();  // Stream the point cloud data

    return 0;
}

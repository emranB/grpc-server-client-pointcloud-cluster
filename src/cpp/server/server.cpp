#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <grpcpp/grpcpp.h>
#include "pointcloud.pb.h"
#include "pointcloud.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;
using namespace pointcloud;

class PointCloudServiceImpl final : public NLPointCloudService::Service {
private:
    double m_eps;
    int m_minSamples;
    int m_maxPercentile;
    int m_minPercentile;

    int duraCount = 0;
    int corticalCount = 0;
    int unknownCount = 0;

public:
    PointCloudServiceImpl(double eps, int minSamples, int maxPercentile, int minPercentile)
        : m_eps(eps), m_minSamples(minSamples), m_maxPercentile(maxPercentile), m_minPercentile(minPercentile) {}

    Status Cluster(ServerContext* context,
                   ServerReaderWriter<NLClusterResponse, NLChunkRequest>* stream) override {
        std::cout << "Received a clustering request." << std::endl;

        NLChunkRequest chunkRequest;
        while (stream->Read(&chunkRequest)) {
            std::cout << "Processing chunk with " << chunkRequest.points_size() << " points." << std::endl;

            // Process each point in the chunk
            for (const auto& point : chunkRequest.points()) {
                NLClusterResponse response;
                response.set_point_id(point.id());

                // Simple dummy clustering logic based on z-value
                if (point.z() > m_maxPercentile) {
                    response.set_label(NLClusterLabel::DURA);
                    duraCount++;
                } else if (point.z() < m_minPercentile) {
                    response.set_label(NLClusterLabel::CORTICAL_SURFACE);
                    corticalCount++;
                } else {
                    response.set_label(NLClusterLabel::UNKNOWN);
                    unknownCount++;
                }

                stream->Write(response);  // Stream the response
            }
        }

        std::cout << "Finished processing the clustering request." << std::endl;

        // Optionally, return a summary after processing all chunks
        NLClusterResponse summaryResponse;
        summaryResponse.set_point_id(-1); // Use -1 to indicate it's a summary
        summaryResponse.set_label(NLClusterLabel::UNKNOWN); // Can be any label for summary

        // Add counts as metadata for summary
        summaryResponse.set_label(NLClusterLabel::DURA); // Dura count
        stream->Write(summaryResponse);  // Stream the summary response

        return Status::OK;
    }
};

// Function to run the gRPC server
void runServer(const std::string& address, double eps, int minSamples, int maxPercentile, int minPercentile) {
    PointCloudServiceImpl service(eps, minSamples, maxPercentile, minPercentile);

    ServerBuilder builder;
    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << address << std::endl;
    server->Wait();
}

int main(int argc, char* argv[]) {
    // Fetch environment variables
    const char* host = std::getenv("HOST");
    const char* port = std::getenv("PORT");
    const char* eps = std::getenv("EPS");
    const char* minSamples = std::getenv("MIN_SAMPLES");
    const char* maxPercentile = std::getenv("MAX_PERCENTILE");
    const char* minPercentile = std::getenv("MIN_PERCENTILE");

    if (!host || !port || !eps || !minSamples || !maxPercentile || !minPercentile) {
        std::cerr << "Environment variables are missing. Please ensure all configurations are set." << std::endl;
        return 1;
    }

    std::string serverAddress = std::string(host) + ":" + std::string(port);
    double epsilon = std::stod(eps);
    int minimumSamples = std::stoi(minSamples);
    int maximumPercentile = std::stoi(maxPercentile);
    int minimumPercentile = std::stoi(minPercentile);

    // Run the server with parameters from environment variables
    runServer(serverAddress, epsilon, minimumSamples, maximumPercentile, minimumPercentile);

    return 0;
}

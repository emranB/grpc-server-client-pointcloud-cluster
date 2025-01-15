#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <grpcpp/grpcpp.h>
#include <fmt/core.h>
#include "pointcloud.pb.h"
#include "pointcloud.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;
using namespace pointcloud;

std::string getTimestamp() {
    std::time_t now = std::time(nullptr);
    char buf[100];
    std::strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", std::localtime(&now));
    return std::string(buf);
}

class PointCloudServiceImpl final : public NLPointCloudService::Service {
private:
    int m_maxPercentile;
    int m_minPercentile;

public:
    PointCloudServiceImpl(int maxPercentile, int minPercentile)
        : m_maxPercentile(maxPercentile), m_minPercentile(minPercentile) {}

    Status Cluster(ServerContext* context,
                   ServerReaderWriter<NLClusterResponse, NLChunkRequest>* stream) override {
        fmt::print("Received a clustering request.\n");

        NLChunkRequest chunkRequest;
        std::vector<float> z_values;
        int totalPointsReceived = 0;
        int totalPointsProcessed = 0;
        int totalPointsDiscarded = 0;

        while (stream->Read(&chunkRequest)) {
            totalPointsReceived += chunkRequest.points_size();
            for (const auto& point : chunkRequest.points()) {
                z_values.push_back(point.z());
            }

            std::sort(z_values.begin(), z_values.end());

            if (z_values.empty()) {
                fmt::print(stderr, "Error: No points received for clustering.\n");
                return Status::CANCELLED;
            }

            float max_z = z_values[static_cast<size_t>((m_maxPercentile / 100.0) * z_values.size())];
            float min_z = z_values[static_cast<size_t>((m_minPercentile / 100.0) * z_values.size())];

            fmt::print("Total points: {}\n", z_values.size());
            fmt::print("Max Z percentile: {:.2f}\n", max_z);
            fmt::print("Min Z percentile: {:.2f}\n", min_z);

            for (const auto& point : chunkRequest.points()) {
                auto response = std::make_unique<NLClusterResponse>();
                response->set_point_id(point.id());

                if (point.z() > max_z) {
                    response->set_label(NLClusterLabel::DURA);
                    totalPointsProcessed++;
                } else if (point.z() < min_z) {
                    response->set_label(NLClusterLabel::CORTICAL_SURFACE);
                    totalPointsProcessed++;
                } else {
                    response->set_label(NLClusterLabel::UNKNOWN);
                    totalPointsProcessed++;
                }

                if (point.z() < min_z || point.z() > max_z) {
                    totalPointsDiscarded++;
                    fmt::print("Point {} discarded with z-value: {:.2f}\n", point.id(), point.z());
                }

                stream->Write(*response);
                fmt::print("Sending back point {} with label {}\n", point.id(), response->label());
            }
        }

        fmt::print("Total points received: {}\n", totalPointsReceived);
        fmt::print("Total points processed: {}\n", totalPointsProcessed);
        fmt::print("Total points discarded: {}\n", totalPointsDiscarded);
        fmt::print("Finished processing the clustering request.\n");
        return Status::OK;
    }
};

void runServer(const std::string& address, int maxPercentile, int minPercentile) {
    auto service = std::make_unique<PointCloudServiceImpl>(maxPercentile, minPercentile);

    ServerBuilder builder;
    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    builder.RegisterService(service.get());

    auto server = builder.BuildAndStart();
    fmt::print("Server listening on {}\n", address);
    server->Wait();
}

int main(int argc, char* argv[]) {
    const char* serverHost = std::getenv("SERVER_HOST");
    const char* serverPort = std::getenv("SERVER_PORT");
    const char* maxPercentile = std::getenv("SERVER_MAX_PERCENTILE");
    const char* minPercentile = std::getenv("SERVER_MIN_PERCENTILE");

    fmt::print("SERVER_HOST: {}\n", serverHost ? serverHost : "not set");
    fmt::print("SERVER_PORT: {}\n", serverPort ? serverPort : "not set");
    fmt::print("SERVER_MAX_PERCENTILE: {}\n", maxPercentile ? maxPercentile : "not set");
    fmt::print("SERVER_MIN_PERCENTILE: {}\n", minPercentile ? minPercentile : "not set");

    if (!serverHost || !serverPort || !maxPercentile || !minPercentile) {
        fmt::print(stderr, "Error: Environment variables are missing. Please ensure all configurations are set.\n");
        return 1;
    }

    std::string serverAddress = fmt::format("{}:{}", serverHost, serverPort);
    int maximumPercentile = std::stoi(maxPercentile);
    int minimumPercentile = std::stoi(minPercentile);

    runServer(serverAddress, maximumPercentile, minimumPercentile);

    return 0;
}

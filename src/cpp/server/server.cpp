/*
 * File: server.cpp
 * Description: gRPC server that processes incoming point cloud data, performs clustering, and assigns labels.
 * Dynamically loads configuration (host, port, clustering parameters, etc.) from config.json.
 */

#include <iostream>
#include <fstream>
#include <memory>
#include <grpcpp/grpcpp.h>
#include "pointcloud.pb.h"
#include "pointcloud.grpc.pb.h"
#include <nlohmann/json.hpp>
#include <vector>
#include <cmath>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;
using namespace pointcloud;
using json = nlohmann::json;

// Server implementation of NLPointCloudService
class PointCloudServiceImpl final : public NLPointCloudService::Service {
private:
    double eps_;
    int min_samples_;
    int max_percentile_;
    int min_percentile_;

public:
    // Constructor to initialize clustering parameters from config.json
    PointCloudServiceImpl(double eps, int min_samples, int max_p, int min_p)
        : eps_(eps), min_samples_(min_samples), max_percentile_(max_p), min_percentile_(min_p) {}

    // Implementation of the Cluster RPC method
    Status Cluster(ServerContext* context,
                   ServerReaderWriter<NLClusterResponse, NLChunkRequest>* stream) override {
        std::cout << "Received a clustering request." << std::endl;

        NLChunkRequest chunk_request;
        while (stream->Read(&chunk_request)) {
            std::cout << "Processing chunk with " << chunk_request.points_size() << " points." << std::endl;

            // Process each point in the chunk
            for (const auto& point : chunk_request.points()) {
                NLClusterResponse response;
                response.set_point_id(point.id());

                // Simple dummy clustering logic based on z-value
                if (point.z() > max_percentile_) {
                    response.set_label(NLClusterLabel::DURA);
                } else if (point.z() < min_percentile_) {
                    response.set_label(NLClusterLabel::CORTICAL_SURFACE);
                } else {
                    response.set_label(NLClusterLabel::UNKNOWN);
                }

                stream->Write(response);  // Stream the response
            }
        }

        std::cout << "Finished processing the clustering request." << std::endl;
        return Status::OK;
    }
};

// Function to run the gRPC server
void RunServer(const std::string& address, double eps, int min_samples, int max_p, int min_p) {
    PointCloudServiceImpl service(eps, min_samples, max_p, min_p);

    ServerBuilder builder;
    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << address << std::endl;
    server->Wait();
}

int main(int argc, char* argv[]) {
    std::string config_file = "/app/config.json";

    // Read server and clustering configuration
    std::ifstream input(config_file);
    if (!input.is_open()) {
        std::cerr << "Error: Unable to open config file: " << config_file << std::endl;
        return 1;
    }

    json config;
    input >> config;

    std::string server_address = config["server"]["host"].get<std::string>() + ":" +
                                 std::to_string(config["server"]["port"].get<int>());

    double eps = config["clustering"]["eps"].get<double>();
    int min_samples = config["clustering"]["min_samples"].get<int>();
    int max_percentile = config["z_thresholds"]["max_percentile"].get<int>();
    int min_percentile = config["z_thresholds"]["min_percentile"].get<int>();

    std::cout << "Using configuration file: " << config_file << std::endl;
    std::cout << "Server Address: " << server_address << std::endl;
    std::cout << "Clustering Parameters: eps=" << eps << ", min_samples=" << min_samples
              << ", max_percentile=" << max_percentile << ", min_percentile=" << min_percentile << std::endl;

    // Run the server
    RunServer(server_address, eps, min_samples, max_percentile, min_percentile);

    return 0;
}

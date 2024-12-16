/*
 * File: client.cpp
 * Description: gRPC client that streams point cloud data from a file to the server in chunks.
 * Dynamically loads configuration from config.json.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <grpcpp/grpcpp.h>
#include "pointcloud.pb.h"
#include "pointcloud.grpc.pb.h"

using json = nlohmann::json;

// gRPC and PointCloud namespaces
using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReaderWriter;
using grpc::Status;
using pointcloud::NLPoint;
using pointcloud::NLChunkRequest;
using pointcloud::NLClusterResponse;
using pointcloud::NLPointCloudService;

// Configuration variables
std::string SERVER_HOST;
int SERVER_PORT;
std::string INPUT_FILE;
int CHUNK_SIZE;

/*
 * loadConfig()
 * Loads client configuration from config.json.
 */
void loadConfig(const std::string& config_path) {
    std::ifstream file(config_path);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to open config file at " << config_path << std::endl;
        exit(1);
    }

    json config;
    file >> config;

    SERVER_HOST = config["client"]["remote_host"];
    SERVER_PORT = config["client"]["remote_port"];
    INPUT_FILE = config["client"]["input_file"];
    CHUNK_SIZE = config["client"]["chunk_size"];

    std::cout << "Loaded Configuration:\n"
              << "Server: " << SERVER_HOST << ":" << SERVER_PORT
              << "\nInput File: " << INPUT_FILE
              << "\nChunk Size: " << CHUNK_SIZE << std::endl;
}

/*
 * PointCloudClient
 * Streams point cloud data to the server and processes responses.
 */
class PointCloudClient {
public:
    PointCloudClient(std::shared_ptr<Channel> channel)
        : stub_(NLPointCloudService::NewStub(channel)) {}

    void StreamPointCloud() {
        ClientContext context;
        std::shared_ptr<ClientReaderWriter<NLChunkRequest, NLClusterResponse>> stream(
            stub_->Cluster(&context));

        std::ifstream infile(INPUT_FILE);
        if (!infile.is_open()) {
            std::cerr << "Error: Failed to open input file: " << INPUT_FILE << std::endl;
            return;
        }

        NLChunkRequest chunk;
        std::string line;
        int point_id = 1;

        // Process file and send points
        while (std::getline(infile, line)) {
            if (line.empty() || line[0] == '#') continue; // Skip comments or invalid lines

            NLPoint* point = chunk.add_points();
            float x, y, z;
            sscanf(line.c_str(), "%f %f %f", &x, &y, &z);
            point->set_x(x);
            point->set_y(y);
            point->set_z(z);
            point->set_id(point_id++);

            if (chunk.points_size() >= CHUNK_SIZE) {
                stream->Write(chunk);
                chunk.Clear();
            }
        }

        if (chunk.points_size() > 0) {
            stream->Write(chunk);
        }
        stream->WritesDone();

        NLClusterResponse response;
        while (stream->Read(&response)) {
            std::cout << "Point ID: " << response.point_id()
                      << ", Label: " << response.label() << std::endl;
        }

        Status status = stream->Finish();
        if (!status.ok()) {
            std::cerr << "Error: gRPC stream failed: " << status.error_message() << std::endl;
        }
    }

private:
    std::unique_ptr<NLPointCloudService::Stub> stub_;
};

int main(int argc, char** argv) {
    std::string config_file = "/app/config.json";
    loadConfig(config_file);

    std::string target = SERVER_HOST + ":" + std::to_string(SERVER_PORT);
    PointCloudClient client(grpc::CreateChannel(target, grpc::InsecureChannelCredentials()));
    std::cout << "Streaming point cloud data to server: " << target << "...\n";
    client.StreamPointCloud();

    return 0;
}

import grpc
import json
import os
from concurrent import futures
from grpc_reflection.v1alpha import reflection
import pointcloud_pb2
import pointcloud_pb2_grpc
import argparse

# Parse command-line arguments
parser = argparse.ArgumentParser()
parser.add_argument("--config", required=True, help="Path to the configuration file")
args = parser.parse_args()

# Load configuration
config_path = args.config
try:
    with open(config_path, "r") as f:
        CONFIG = json.load(f)
    print(f"Configuration loaded successfully from {config_path}")
except FileNotFoundError:
    print(f"Error: Configuration file not found at {config_path}")
    exit(1)
except json.JSONDecodeError as e:
    print(f"Error: Failed to parse JSON configuration file: {e}")
    exit(1)

# Define the gRPC server
class PointCloudService(pointcloud_pb2_grpc.NLPointCloudServiceServicer):
    def Cluster(self, request_iterator, context):
        print("Received a clustering request.")  # Debug log for requests
        # Add your clustering logic here
        return iter([])  # Empty response for now

def serve():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))

    # Add service to the server
    pointcloud_pb2_grpc.add_NLPointCloudServiceServicer_to_server(PointCloudService(), server)

    # Enable reflection
    SERVICE_NAMES = (
        pointcloud_pb2.DESCRIPTOR.services_by_name["NLPointCloudService"].full_name,
        reflection.SERVICE_NAME,
    )
    reflection.enable_server_reflection(SERVICE_NAMES, server)

    # Bind server to the specified host and port
    address = f'{CONFIG["server"]["host"]}:{CONFIG["server"]["port"]}'
    print(f"Starting server on {address}...")  # Log the server address
    server.add_insecure_port(address)

    # Start the server
    server.start()
    print("Server is running and ready to accept connections on {address}.")  # Confirmation log
    server.wait_for_termination()

if __name__ == "__main__":
    print("Initializing PointCloud Server...")
    serve()

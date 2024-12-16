import grpc
import pointcloud_pb2
import pointcloud_pb2_grpc
import json
import argparse
import os


def read_pointcloud(file_path, chunk_size):
    """
    Reads the point cloud data from a file and yields chunks for streaming.
    """
    points = []
    header_passed = False

    try:
        with open(file_path, "r") as f:
            for line in f:
                line = line.strip()

                # Skip header lines until "DATA" is found
                if not header_passed:
                    if line.startswith("DATA"):
                        header_passed = True
                    continue

                # Process point data lines
                try:
                    x, y, z = map(float, line.split())
                    points.append(pointcloud_pb2.NLPoint(x=x, y=y, z=z, id=len(points) + 1))
                except ValueError:
                    print(f"Skipping invalid line: {line}")
                    continue

        # Yield chunks of points
        for i in range(0, len(points), chunk_size):
            chunk = pointcloud_pb2.NLChunkRequest(points=points[i:i + chunk_size])
            print(f"Sending chunk with {len(chunk.points)} points.")
            yield chunk

    except FileNotFoundError:
        print(f"Error: File {file_path} not found.")
        raise
    except Exception as e:
        print(f"Unexpected error while reading point cloud data: {e}")
        raise


def analyze_results(points, labels):
    """
    Analyzes and reports the number and percentage of each label in the results.
    """
    total_points = len(points)
    counts = {label: labels.count(label) for label in set(labels)}

    for label, count in counts.items():
        percentage = (count / total_points) * 100
        print(f"Label {label}: {count} points ({percentage:.2f}%)")


def run(config, host, port, input_file, output_file):
    """
    Connects to the gRPC server and streams point cloud data.
    """
    target = f"{host}:{port}"
    chunk_size = config.get("client", {}).get("chunk_size", 500)
    labels_file = output_file

    print(f"Connecting to gRPC server at {target} (plaintext)...")
    channel = grpc.insecure_channel(target)
    stub = pointcloud_pb2_grpc.NLPointCloudServiceStub(channel)

    received_labels = []
    points = []

    try:
        responses = stub.Cluster(read_pointcloud(input_file, chunk_size))
        for response in responses:
            print(f"Point ID: {response.point_id}, Label: {response.label}")
            points.append(response.point_id)
            received_labels.append(response.label)
    except grpc.RpcError as e:
        print(f"gRPC Error: {e.details()} (code: {e.code()})")
        return

    if config["debug"]["enable"]:
        print("\n****************** DEBUG ENABLED **********************")

        # Results Generated
        print("\nResults Generated:")
        analyze_results(points, received_labels)

        # Results Expected
        print("\nResults Expected:")
        try:
            with open(labels_file, "r") as f:
                expected_labels = [int(line.strip()) for line in f.readlines()]
            if len(expected_labels) == len(points):
                analyze_results(points, expected_labels)
            else:
                print("Mismatch between the number of points and expected labels. Verify input files.")
        except FileNotFoundError:
            print(f"Error: Labels file {labels_file} not found.")
        except Exception as e:
            print(f"Unexpected error while reading labels file: {e}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--config", help="Path to the configuration file")
    parser.add_argument("--host", help="gRPC server host")
    parser.add_argument("--port", help="gRPC server port")
    parser.add_argument("--input", help="Input file path")
    parser.add_argument("--output", help="Output file path")
    args = parser.parse_args()

    config = {}
    if args.config:
        try:
            with open(args.config, "r") as f:
                config = json.load(f)
            print(f"Configuration loaded successfully from {args.config}")
        except Exception as e:
            print(f"Error loading configuration: {e}")
            exit(1)

    # Use arguments or fallback to configuration
    host = args.host or config.get("client", {}).get("remote_host", "localhost")
    port = args.port or config.get("client", {}).get("remote_port", 1111)
    input_file = args.input or config.get("client", {}).get("input_file", "./data/input.pcd")
    output_file = args.output or config.get("debug", {}).get("input_labels_file", "./data/expected.labels")

    run(config, host, port, input_file, output_file)

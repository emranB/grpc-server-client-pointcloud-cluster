#!/bin/bash
# Build and run the gRPC client with argument parsing for host, port, input, and output

set -e

# Parse arguments
while [[ "$#" -gt 0 ]]; do
    case $1 in
        --host) HOST="$2"; shift ;;
        --port) PORT="$2"; shift ;;
        --input) INPUT="$2"; shift ;;
        --output) OUTPUT="$2"; shift ;;
        *) echo "Unknown parameter: $1" >&2; exit 1 ;;
    esac
    shift
done

# Ensure all arguments are provided
if [[ -z "$HOST" || -z "$PORT" || -z "$INPUT" || -z "$OUTPUT" ]]; then
    echo "Usage: $0 --host <HOST> --port <PORT> --input <INPUT_FILE> --output <OUTPUT_FILE>"
    exit 1
fi

# Export arguments as environment variables for the client
export CLIENT_REMOTE_HOST="$HOST"
export CLIENT_REMOTE_PORT="$PORT"
export CLIENT_INPUT_FILE="$INPUT"
export CLIENT_OUTPUT_FILE="$OUTPUT"

# Debug: Log the arguments
echo "Running client with arguments:"
echo "HOST=${CLIENT_REMOTE_HOST}"
echo "PORT=${CLIENT_REMOTE_PORT}"
echo "INPUT=${CLIENT_INPUT_FILE}"
echo "OUTPUT=${CLIENT_OUTPUT_FILE}"

# Generate protobuf and gRPC files
mkdir -p /app/src/cpp/client/build
protoc -I /app \
    --cpp_out=/app/src/cpp/client/build \
    --grpc_out=/app/src/cpp/client/build \
    --plugin=protoc-gen-grpc=/usr/local/bin/grpc_cpp_plugin \
    /app/pointcloud.proto

# Build the client
cd /app/src/cpp/client/build
cmake -DCMAKE_PREFIX_PATH=/usr/local ..
make -j$(nproc)

# Run the client
./client --host "$CLIENT_REMOTE_HOST" --port "$CLIENT_REMOTE_PORT" --input "$CLIENT_INPUT_FILE" --output "$CLIENT_OUTPUT_FILE"

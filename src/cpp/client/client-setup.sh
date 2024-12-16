#!/bin/bash

# Description: Sets up the C++ client environment, compiles the gRPC files, and builds the client binary.
set -e  # Exit immediately if a command fails

echo "Setting up the C++ client..."

# Ensure build directory exists
mkdir -p /app/build

# Generate gRPC and Protobuf code
echo "Generating gRPC and protobuf files..."
protoc -I /app \
  --cpp_out=/app/build \
  --grpc_out=/app/build \
  --plugin=protoc-gen-grpc=/usr/local/bin/grpc_cpp_plugin \
  /app/pointcloud.proto

echo "Generated gRPC and protobuf files successfully."

# Compile the client binary
echo "Compiling the C++ client..."
g++ -std=c++17 -o /app/client \
  /app/client.cpp \
  /app/build/pointcloud.pb.cc \
  /app/build/pointcloud.grpc.pb.cc \
  -I/usr/local/include -L/usr/local/lib \
  -lgrpc++ -lgrpc -lprotobuf -lpthread

echo "C++ client compiled successfully."

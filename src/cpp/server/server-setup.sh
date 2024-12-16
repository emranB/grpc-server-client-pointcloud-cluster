#!/bin/bash

# Description: Sets up the C++ server environment and compiles the gRPC files.

set -e  # Exit immediately if a command fails

echo "Setting up the C++ server..."

# Step 1: Create a build directory for generated files
mkdir -p /app/build

# Step 2: Generate gRPC and protobuf code
protoc -I /app \
  --cpp_out=/app/build \
  --grpc_out=/app/build \
  --plugin=protoc-gen-grpc=/usr/local/bin/grpc_cpp_plugin \
  /app/pointcloud.proto

echo "Generated gRPC and protobuf files successfully."

# Step 3: Compile the C++ server
g++ -std=c++17 -o /app/server \
    /app/server.cpp \
    /app/build/pointcloud.pb.cc \
    /app/build/pointcloud.grpc.pb.cc \
    -I/usr/local/include \
    -I/app/build \
    -L/usr/local/lib \
    -lgrpc++ -lgrpc -lprotobuf -labsl_bad_variant_access -labsl_base -labsl_time \
    -labsl_strings -labsl_int128 -lpthread -ldl

echo "C++ server compiled successfully."

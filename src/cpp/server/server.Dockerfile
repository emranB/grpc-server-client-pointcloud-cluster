# Server Dockerfile
# Description: Builds the server container for the C++ gRPC application.
# Installs dependencies, compiles protobuf and gRPC files, and builds the server executable.

FROM ubuntu:22.04

# Set the working directory
WORKDIR /app

# Install system dependencies
RUN apt-get update && apt-get install -y \
    build-essential cmake git wget curl \
    libprotobuf-dev protobuf-compiler \
    zlib1g-dev libssl-dev libabsl-dev \
    && apt-get clean

# Build and install nlohmann/json from source
RUN git clone https://github.com/nlohmann/json.git /tmp/json && \
    cd /tmp/json && \
    mkdir -p build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    make install && \
    ldconfig

# Build and install gRPC from source
RUN git clone --recurse-submodules -b v1.55.0 https://github.com/grpc/grpc.git /tmp/grpc && \
    cd /tmp/grpc && \
    mkdir -p cmake/build && cd cmake/build && \
    cmake -DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release ../.. && \
    make -j$(nproc) && make install && \
    ldconfig

# Copy project files
COPY src/cpp/server/server.cpp /app/server.cpp
COPY src/cpp/server/server-setup.sh /app/server-setup.sh
COPY src/cpp/server/server.sh /app/server.sh
COPY src/cpp/server/CMakeLists.txt /app/CMakeLists.txt
COPY pointcloud.proto /app/pointcloud.proto

# Make scripts executable
RUN chmod +x /app/server-setup.sh /app/server.sh

# Run server setup script to generate and compile protobuf/gRPC files
RUN ./server-setup.sh

# Set the entrypoint to server.sh
ENTRYPOINT ["/app/server.sh", "--config=/app/config.json"]

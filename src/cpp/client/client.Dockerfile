# Base image
FROM ubuntu:22.04

# Set working directory
WORKDIR /app

# Install system dependencies
RUN apt-get update && apt-get install -y \
    build-essential cmake git wget curl \
    libprotobuf-dev protobuf-compiler \
    pkg-config autoconf libtool \
    && apt-get clean

# Install gRPC (build from source)
RUN git clone --recurse-submodules -b v1.55.0 https://github.com/grpc/grpc.git /tmp/grpc && \
    cd /tmp/grpc && mkdir -p cmake/build && cd cmake/build && \
    cmake -DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release ../.. && \
    make -j$(nproc) && make install && ldconfig

# Install nlohmann/json
RUN wget -q https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp -O /usr/local/include/nlohmann/json.hpp

# Copy project files
COPY client.cpp /app/client.cpp
COPY client-setup.sh /app/client-setup.sh
COPY client.sh /app/client.sh
COPY ../../pointcloud.proto /app/pointcloud.proto
COPY ../server/CMakeLists.txt /app/CMakeLists.txt
COPY config.json /app/config.json

# Make scripts executable
RUN chmod +x /app/client-setup.sh /app/client.sh

# Run setup script
RUN ./client-setup.sh

# Default entrypoint
ENTRYPOINT ["/app/client.sh", "--config=/app/config.json"]

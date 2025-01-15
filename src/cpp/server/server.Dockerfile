# Use Ubuntu 22.04 as the base image
FROM ubuntu:22.04

# Set non-interactive installation mode
ENV DEBIAN_FRONTEND=noninteractive

# Install necessary dependencies and gRPC dependencies
RUN apt-get update && apt-get install -y \
    git \
    cmake \
    build-essential \
    autoconf \
    libtool \
    pkg-config \
    wget \
    protobuf-compiler \
    libprotobuf-dev \
    libfmt-dev \
    && rm -rf /var/lib/apt/lists/*

# Clone the gRPC repository and its submodules
RUN git clone --recurse-submodules -b v1.66.0 https://github.com/grpc/grpc

# Build and install gRPC and its dependencies
WORKDIR /grpc/cmake/build
RUN cmake -DgRPC_INSTALL=ON \
          -DgRPC_BUILD_TESTS=OFF \
          -DCMAKE_INSTALL_PREFIX=/usr/local \
          ../.. \
    && make -j$(nproc) \
    && make install

# Set the working directory for the application
WORKDIR /app

# Copy the necessary application files
COPY . /app

# Generate the .pb.cc and .pb.h files from pointcloud.proto using protoc
RUN mkdir -p /app/src/cpp/server/build && \
    protoc -I=/app --cpp_out=/app/src/cpp/server/build /app/pointcloud.proto && \
    protoc -I=/app --grpc_out=/app/src/cpp/server/build --plugin=protoc-gen-grpc=/usr/local/bin/grpc_cpp_plugin /app/pointcloud.proto

# Build the server
RUN cd /app/src/cpp/server/build && \
    cmake -DCMAKE_PREFIX_PATH=/usr/local .. && \
    make -j$(nproc)

# Run the server directly (no entrypoint needed)
CMD ["/app/src/cpp/server/build/server"]

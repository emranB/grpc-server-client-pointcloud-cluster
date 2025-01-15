# Use Ubuntu 22.04 as the base image
FROM ubuntu:22.04

# Set non-interactive installation mode
ENV DEBIAN_FRONTEND=noninteractive

# Set the working directory for the application
WORKDIR /app

# Copy the application files, including shell scripts, into the container
COPY . /app

# Make sure client.sh is executable
RUN chmod +x /app/src/cpp/client/client-setup.sh

# Make sure client.sh is executable
RUN /app/src/cpp/client/client-setup.sh

# # Install necessary packages
# RUN apt-get update && \
#     apt-get install -y \
#     git \
#     cmake \
#     build-essential \
#     autoconf \
#     libtool \
#     pkg-config \
#     wget \
#     protobuf-compiler \
#     libprotobuf-dev \
#     libfmt-dev \
#     && rm -rf /var/lib/apt/lists/*

# # Clone and build gRPC
# RUN git clone --recurse-submodules -b v1.66.0 https://github.com/grpc/grpc /grpc && \
#     mkdir -p /grpc/cmake/build && \
#     cd /grpc/cmake/build && \
#     cmake -DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX=/usr/local ../.. && \
#     make -j$(nproc) && \
#     make install

# Make sure client.sh is executable
RUN chmod +x /app/src/cpp/client/client.sh

# # Generate protobuf and gRPC files (if not already generated)
# RUN mkdir -p /app/src/cpp/client/build && \
#     protoc -I /app --cpp_out=/app/src/cpp/client/build --grpc_out=/app/src/cpp/client/build --plugin=protoc-gen-grpc=/usr/local/bin/grpc_cpp_plugin /app/pointcloud.proto

# # Build the client if not already built
# WORKDIR /app/src/cpp/client/build
# RUN cmake -DCMAKE_PREFIX_PATH=/usr/local .. && \
#     make -j$(nproc)

# Run the client.sh script with environment variables (set via docker-compose)
CMD ["bash", "-c", "echo 'HOST=$CLIENT_REMOTE_HOST'; echo 'PORT=$CLIENT_REMOTE_PORT'; echo 'INPUT=$CLIENT_INPUT_FILE'; echo 'OUTPUT=$CLIENT_OUTPUT_FILE'; /app/src/cpp/client/client.sh --host $CLIENT_REMOTE_HOST --port $CLIENT_REMOTE_PORT --input $CLIENT_INPUT_FILE --output $CLIENT_OUTPUT_FILE"]

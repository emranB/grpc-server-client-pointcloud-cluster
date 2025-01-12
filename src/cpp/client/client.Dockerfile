# Use Ubuntu 22.04 as the base image
FROM ubuntu:22.04

# Set non-interactive installation mode
ENV DEBIAN_FRONTEND=noninteractive

# Install necessary dependencies
RUN apt-get update && apt-get install -y \
    curl \
    wget \
    git \
    build-essential \
    cmake \
    protobuf-compiler \
    libprotobuf-dev \
    && rm -rf /var/lib/apt/lists/*

# Copy client-setup.sh and client.sh from src/cpp/client into /app in the container
COPY ./src/cpp/client/client-setup.sh /app/client-setup.sh
COPY ./src/cpp/client/client.sh /app/client.sh

# Make scripts executable
RUN chmod +x /app/client-setup.sh /app/client.sh

# Run the setup script
RUN /app/client-setup.sh

# Set the working directory
WORKDIR /app

# Copy the rest of the application files (including pointcloud.proto) into the container
COPY . /app

# Run the client
CMD ["sh", "-c", "/app/client.sh --host \"$CLIENT_REMOTE_HOST\" --port \"$CLIENT_REMOTE_PORT\" --input \"$CLIENT_INPUT_FILE\" --output \"$CLIENT_OUTPUT_FILE\""]

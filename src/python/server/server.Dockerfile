FROM ubuntu:22.04

# Set working directory
WORKDIR /app

# Install system dependencies
RUN apt-get update && apt-get install -y \
    python3 python3-pip jq net-tools \
    && apt-get clean

RUN apt-get update && apt-get install -y curl && \
    curl -L https://github.com/fullstorydev/grpcurl/releases/download/v1.8.7/grpcurl_1.8.7_linux_x86_64.tar.gz | tar -xz -C /usr/local/bin && \
    chmod +x /usr/local/bin/grpcurl

# Copy project files
COPY src/python/server/server-setup.sh /app/server-setup.sh
COPY src/python/server/server.sh /app/server.sh
COPY src/python/server/server.py /app/server.py
COPY pointcloud.proto /app/pointcloud.proto
COPY config.json /app/config.json
COPY requirements.txt /app/requirements.txt

# Make scripts executable
RUN chmod +x /app/server-setup.sh /app/server.sh

# Run server setup
RUN /app/server-setup.sh

# Reset ENTRYPOINT to avoid conflicts
ENTRYPOINT []

# Properly set CMD to call the script with the config argument
CMD ["/app/server.sh", "--config=/app/config.json"]

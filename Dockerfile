FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    python3 python3-pip \
    netcat \
    curl \
    iputils-ping \
    && apt-get clean

# Install grpcurl for debugging
RUN curl -L https://github.com/fullstorydev/grpcurl/releases/download/v1.8.7/grpcurl_1.8.7_linux_x86_64.tar.gz | \
    tar -xz -C /usr/local/bin grpcurl

# Install Python dependencies
COPY requirements.txt /app/requirements.txt
RUN pip3 install -r /app/requirements.txt

# Copy files
COPY src /app/src
COPY pointcloud.proto /app/pointcloud.proto
COPY config.json /app/config.json
COPY data /app/data

# Generate gRPC Python files
RUN python3 -m grpc_tools.protoc -I/app --python_out=/app/src/python --grpc_python_out=/app/src/python /app/pointcloud.proto

# Add PYTHONPATH to include generated gRPC files
ENV PYTHONPATH="/app/src/python"

# Set working directory
WORKDIR /app/src/python

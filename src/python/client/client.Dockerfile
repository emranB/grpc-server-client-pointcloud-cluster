FROM ubuntu:22.04

# Set working directory
WORKDIR /app

# Install system dependencies
RUN apt-get update && apt-get install -y \
    python3 python3-pip jq net-tools curl && \
    curl -L https://github.com/fullstorydev/grpcurl/releases/download/v1.8.7/grpcurl_1.8.7_linux_x86_64.tar.gz | tar -xz -C /usr/local/bin && \
    chmod +x /usr/local/bin/grpcurl && \
    apt-get clean

# Copy project files
COPY src/python/client/client-setup.sh /app/client-setup.sh
COPY src/python/client/client.sh /app/client.sh
COPY src/python/client/client.py /app/client.py
COPY config.json /app/config.json
COPY requirements.txt /app/requirements.txt

# Make scripts executable
RUN chmod +x /app/client-setup.sh /app/client.sh

# Run client setup
RUN /app/client-setup.sh

# Set ENTRYPOINT to allow flexibility
ENTRYPOINT ["/app/client.sh"]

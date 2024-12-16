# server-setup.sh

#!/bin/bash

# Check if running as root (inside Docker)
if [ "$(id -u)" -eq 0 ]; then
    INSTALL_CMD="apt-get"
else
    INSTALL_CMD="sudo apt-get"
fi

# Install system dependencies
echo "Installing required system packages..."
$INSTALL_CMD update && $INSTALL_CMD install -y jq python3 python3-pip

# Install Python dependencies
echo "Setting up server environment..."
pip3 install -r /app/requirements.txt
python3 -m grpc_tools.protoc -I/app --python_out=/app --grpc_python_out=/app /app/pointcloud.proto

# Ensure server script is executable
chmod +x /app/server.sh

echo "Server setup completed."


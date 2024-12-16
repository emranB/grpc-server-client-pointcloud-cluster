# client-setup.sh

#!/bin/bash

# Check if running as root (inside Docker)
if [ "$(id -u)" -eq 0 ]; then
    INSTALL_CMD="apt-get"
else
    INSTALL_CMD="sudo apt-get"
fi

# Install dependencies
echo "Installing required packages..."
$INSTALL_CMD update && $INSTALL_CMD install -y jq python3 python3-pip

# Install Python dependencies
echo "Setting up client environment..."
pip3 install -r /app/requirements.txt

# Ensure client wrapper script is executable
chmod +x /app/src/python/client/client.sh

echo "Client setup completed."

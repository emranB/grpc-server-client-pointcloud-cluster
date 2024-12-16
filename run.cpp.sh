#!/bin/bash

# File: run.cpp.sh
# Description: Safely manages the C++ Docker Compose lifecycle.
# Stops containers, cleans unused resources, rebuilds, and runs the C++ client-server setup.

set -e  # Exit immediately if any command fails

# Ensure the script is run from the root directory
if [[ ! -f "docker-compose.cpp.yml" ]]; then
    echo "Error: docker-compose.cpp.yml not found in the current directory."
    echo "Please run this script from the root of your project."
    exit 1
fi

# Step 1: Stop existing containers
echo "Stopping and cleaning up existing Docker containers..."
docker compose -f docker-compose.cpp.yml down

# Step 2: Prune unused Docker resources (volumes, networks, containers)
echo "Cleaning up unused Docker resources..."
docker system prune -f --volumes

# Step 3: Build and start the Docker containers
echo "Building and starting the C++ client-server setup..."
docker compose -f docker-compose.cpp.yml up --build

echo "C++ client-server setup is now running."

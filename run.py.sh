#!/bin/bash

# Exit on any failure
set -e

# Ensure we are running the script in the correct directory
if [[ ! -f "docker-compose.python.yml" ]]; then
    echo "Error: docker-compose.python.yml not found in the current directory."
    echo "Please run this script from the root of your project."
    exit 1
fi

# Check if config.json exists
if [[ ! -f "./config.json" ]]; then
    echo "Error: config.json not found in the current directory."
    exit 1
fi

# Stop and clean up existing containers
echo "Stopping and cleaning up Docker containers..."
docker compose -f docker-compose.python.yml down || true
docker system prune -f --volumes || true

# Build and run the Docker Compose services
echo "Building and starting services..."
docker compose -f docker-compose.python.yml up --build

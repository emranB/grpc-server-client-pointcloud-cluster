#!/bin/bash

# Default to "oct-2-small" files if no arguments are given
DEFAULT_PCD_FILE="oct-2-medium.pcd"
DEFAULT_LABELS_FILE="oct-2-medium.labels"

PCD_FILE="${1:-$DEFAULT_PCD_FILE}"
LABELS_FILE="${2:-$DEFAULT_LABELS_FILE}"

# Debug: Print out the file paths being used
echo "Using PCD file: ${PCD_FILE}"
echo "Using labels file: ${LABELS_FILE}"

# Step 1: Build the Docker image (from inside the 'help/' folder).
echo "Building Docker image..."
docker-compose up --build --no-start

# Step 2: Run the container with the filenames passed as arguments
echo "Running the container with input/output file names..."
docker-compose run visualize_tool $PCD_FILE $LABELS_FILE

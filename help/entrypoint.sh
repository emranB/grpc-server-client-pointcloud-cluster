#!/bin/bash

# Default to "oct-2-small" files if no arguments are given
DEFAULT_PCD_FILE="oct-2-small.pcd"
DEFAULT_LABELS_FILE="oct-2-small.labels"

# Accept input arguments for PCD and labels, or use defaults
PCD_FILE="${1:-$DEFAULT_PCD_FILE}"
LABELS_FILE="${2:-$DEFAULT_LABELS_FILE}"

# Set the base path inside the container
BASE_PATH="/app/data"

# Construct the full paths
FULL_PCD_FILE="${BASE_PATH}/${PCD_FILE}"
FULL_LABELS_FILE="${BASE_PATH}/${LABELS_FILE}"

# Debug: Print out the full file paths being used
echo "Using PCD file: ${FULL_PCD_FILE}"
echo "Using labels file: ${FULL_LABELS_FILE}"

# Run the Python script with the specified file paths
python3 /app/visualize.py -i "$FULL_PCD_FILE" -l "$FULL_LABELS_FILE"

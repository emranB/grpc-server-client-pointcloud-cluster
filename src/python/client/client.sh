#!/bin/bash

# File: client.sh
# Description: Wrapper script for the C++ client binary. It parses command-line arguments
#              and validates required inputs before executing the client.
# Usage: ./client.sh --config=<path_to_config> OR provide all required arguments.

set -e  # Exit immediately if any command fails

# Default values
CONFIG_FILE=""
HOST=""
PORT=""
INPUT=""
OUTPUT=""

# Parse input arguments
while [[ "$#" -gt 0 ]]; do
    case $1 in
        --config=*) CONFIG_FILE="${1#*=}"; shift ;;
        --host=*) HOST="${1#*=}"; shift ;;
        --port=*) PORT="${1#*=}"; shift ;;
        --input=*) INPUT="${1#*=}"; shift ;;
        --output=*) OUTPUT="${1#*=}"; shift ;;
        *) echo "Unknown argument: $1"; exit 1 ;;
    esac
done

# If a config file is provided, load values from it
if [[ -n "$CONFIG_FILE" ]]; then
    if [[ ! -f "$CONFIG_FILE" ]]; then
        echo "Error: Config file not found at $CONFIG_FILE"
        exit 1
    fi

    HOST=$(jq -r '.client.remote_host' "$CONFIG_FILE")
    PORT=$(jq -r '.client.remote_port' "$CONFIG_FILE")
    INPUT=$(jq -r '.client.input_file' "$CONFIG_FILE")
    OUTPUT=$(jq -r '.debug.input_labels_file' "$CONFIG_FILE")
fi

# Validate required arguments
if [[ -z "$HOST" || -z "$PORT" || -z "$INPUT" || -z "$OUTPUT" ]]; then
    echo "Error: Missing required arguments. Provide --config or all of --host, --port, --input, and --output."
    exit 1
fi

# Log the configuration being used
echo "Using configuration:"
echo "Host: $HOST"
echo "Port: $PORT"
echo "Input File: $INPUT"
echo "Output File: $OUTPUT"

# Run the C++ client binary with the validated arguments
exec ./client --host="$HOST" --port="$PORT" --input="$INPUT" --output="$OUTPUT"

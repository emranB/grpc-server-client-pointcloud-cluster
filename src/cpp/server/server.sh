#!/bin/bash

# File: server.sh
# Description: Wrapper script for starting the C++ gRPC server. Parses command-line arguments,
#              validates input, and launches the server binary with the appropriate configuration.
# Usage: ./server.sh --config=<path_to_config>
#
# If --config is not provided, the script will throw an error.

set -e  # Exit immediately if any command fails

# Default values
CONFIG_FILE=""

# Parse input arguments
while [[ "$#" -gt 0 ]]; do
    case $1 in
        --config=*) CONFIG_FILE="${1#*=}"; shift ;;
        *) echo "Unknown argument: $1"; exit 1 ;;
    esac
done

# Validate configuration file
if [[ -z "$CONFIG_FILE" ]]; then
    echo "Error: --config=<path_to_config> is required."
    exit 1
fi

if [[ ! -f "$CONFIG_FILE" ]]; then
    echo "Error: Config file not found at $CONFIG_FILE"
    exit 1
fi

# Load values from the config file
HOST=$(jq -r '.server.host' "$CONFIG_FILE")
PORT=$(jq -r '.server.port' "$CONFIG_FILE")

# Validate required values
if [[ -z "$HOST" || -z "$PORT" ]]; then
    echo "Error: Missing required server configuration in $CONFIG_FILE."
    exit 1
fi

# Log the configuration being used
echo "Starting gRPC Server with the following configuration:"
echo "Host: $HOST"
echo "Port: $PORT"

# Launch the C++ server binary
exec ./server --host="$HOST" --port="$PORT"

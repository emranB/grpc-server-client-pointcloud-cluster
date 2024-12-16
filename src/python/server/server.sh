#!/bin/bash

# Default config file
CONFIG_FILE=""

# Parse arguments
while [[ "$#" -gt 0 ]]; do
    case $1 in
        --config=*) CONFIG_FILE="${1#*=}"; shift ;;
        *) echo "Unknown argument: $1"; exit 1 ;;
    esac
done

# Validate config file
if [[ -z "$CONFIG_FILE" ]]; then
    echo "Error: --config=<config_file> is required."
    exit 1
fi

echo "Using configuration file: $CONFIG_FILE"

# Forward arguments to the Python script
exec python3 /app/server.py --config="$CONFIG_FILE"

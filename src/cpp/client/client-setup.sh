#!/bin/bash
# Client setup script for building gRPC and client

set -e

# Update and install necessary packages
apt-get update
apt-get install -y \
    git \
    cmake \
    build-essential \
    autoconf \
    libtool \
    pkg-config \
    wget \
    protobuf-compiler \
    libprotobuf-dev
rm -rf /var/lib/apt/lists/*

# Clone and build gRPC
git clone --recurse-submodules -b v1.66.0 https://github.com/grpc/grpc
mkdir -p /grpc/cmake/build
cd /grpc/cmake/build
cmake -DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX=/usr/local ../..
make -j$(nproc)
make install

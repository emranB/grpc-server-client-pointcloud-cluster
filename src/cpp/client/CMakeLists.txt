cmake_minimum_required(VERSION 3.15)

# Project name and language
project(PointCloudClient LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Debug: Print source directory
message(STATUS "CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR}")

# Locate Protobuf and gRPC libraries
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/build)

# Define source paths explicitly
set(PROTO_SRCS ${CMAKE_SOURCE_DIR}/build/pointcloud.pb.cc ${CMAKE_SOURCE_DIR}/build/pointcloud.grpc.pb.cc)
set(PROTO_HDRS ${CMAKE_SOURCE_DIR}/build/pointcloud.pb.h ${CMAKE_SOURCE_DIR}/build/pointcloud.grpc.pb.h)
set(CLIENT_CPP ${CMAKE_SOURCE_DIR}/client.cpp)

# Debug: Print resolved paths
message(STATUS "PROTO_SRCS=${PROTO_SRCS}")
message(STATUS "PROTO_HDRS=${PROTO_HDRS}")
message(STATUS "CLIENT_CPP=${CLIENT_CPP}")

# Add the client executable
add_executable(client ${CLIENT_CPP} ${PROTO_SRCS})

# Link Protobuf and gRPC libraries
target_link_libraries(client gRPC::grpc++ protobuf::libprotobuf)

cmake_minimum_required(VERSION 3.15)

# Project name and language
project(PointCloudService LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Locate Protobuf, gRPC, and fmt libraries
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(fmt REQUIRED)  # Add fmt dependency

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/build)

# Define source paths explicitly
set(PROTO_SRCS ${CMAKE_SOURCE_DIR}/build/pointcloud.pb.cc ${CMAKE_SOURCE_DIR}/build/pointcloud.grpc.pb.cc)
set(PROTO_HDRS ${CMAKE_SOURCE_DIR}/build/pointcloud.pb.h ${CMAKE_SOURCE_DIR}/build/pointcloud.grpc.pb.h)
set(SERVER_CPP ${CMAKE_SOURCE_DIR}/server.cpp)

# Debug: Print resolved paths
message(STATUS "PROTO_SRCS=${PROTO_SRCS}")
message(STATUS "PROTO_HDRS=${PROTO_HDRS}")
message(STATUS "SERVER_CPP=${SERVER_CPP}")

# Add the server executable
add_executable(server ${SERVER_CPP} ${PROTO_SRCS})

# Link Protobuf, gRPC, and fmt libraries
target_link_libraries(server gRPC::grpc++ protobuf::libprotobuf fmt::fmt)  # Link fmt here

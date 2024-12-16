cmake_minimum_required(VERSION 3.15)

# Project name and language
project(PointCloudService LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# FetchContent for gRPC and nlohmann/json
include(FetchContent)

# Fetch nlohmann/json
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp
)
FetchContent_MakeAvailable(json)

# Fetch gRPC
FetchContent_Declare(
    grpc
    GIT_REPOSITORY https://github.com/grpc/grpc.git
    GIT_TAG v1.55.0
)
FetchContent_MakeAvailable(grpc)

# Find Protobuf and gRPC
find_package(Protobuf REQUIRED)
find_package(gRPC CONFIG REQUIRED)

# Protobuf and gRPC generation
set(PROTO_FILE ${CMAKE_SOURCE_DIR}/../../pointcloud.proto)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILE})
grpc_generate_cpp(GRPC_SRCS GRPC_HDRS ${PROTO_FILE})

# Add server executable
add_executable(server server.cpp ${PROTO_SRCS} ${GRPC_SRCS})
target_include_directories(server PRIVATE ${Protobuf_INCLUDE_DIRS})
target_link_libraries(server PRIVATE gRPC::grpc++ protobuf::libprotobuf)

# Add client executable
add_executable(client ../client/client.cpp ${PROTO_SRCS} ${GRPC_SRCS})
target_include_directories(client PRIVATE ${Protobuf_INCLUDE_DIRS})
target_link_libraries(client PRIVATE gRPC::grpc++ protobuf::libprotobuf)

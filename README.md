# gRPC Point Cloud Clustering Service

## Problem Overview

The goal is to implement a gRPC service for clustering point clouds from brain scans into three distinct groups:
1. **Dura**: Top layer of the scan.
2. **Cortical Surface**: Bottom layer of the scan.
3. **Unknown**: Noise or artifacts between the layers.

**Key Assumption**: The *negative-Z* direction is always *up* in the point cloud data.

The input point cloud data is provided as `.PCD` files, and the output is a text file mapping each point ID to its cluster label.

---

## Deliverables

### 1. Docker Configuration
- A `Dockerfile` to build and run the gRPC server.
- The server must expose port `50051`.

### 2. Server Implementation
- A gRPC server implementing the `NLPointCloudService` interface defined in `pointcloud.proto`.
- The `Cluster` RPC endpoint should:
  - Accept streamed point cloud data from the client.
  - Perform clustering and return cluster assignments for each point ID as a stream.

### 3. Client Implementation
- A client that supports the following arguments:
  - `--host`: Server hostname or IP.
  - `--port`: Server port.
  - `--input`: Path to the `.PCD` file.
  - `--output`: Path to the output text file containing cluster labels.
- The client should:
  - Read the `.PCD` file and stream points to the server.
  - Save the cluster labels received from the server in the specified output file.

### 4. Client Setup Script
- A `client-setup.sh` script that prepares the client to run on a fresh Ubuntu 22.04 installation.
- The client should be executed using the command: `./client`.

### 5. Output Requirements
- The output label file should map:
  - **Point ID (line number)** → **Cluster label (Dura, Cortical Surface, Unknown)**.

---

## Solution Requirements

### Server
- Implement the `Cluster` RPC endpoint using a clustering algorithm of your choice.
- The server should be capable of handling streamed data and provide cluster labels as a streamed response.

### Client
- Assign unique IDs to points in the client.
- Stream point cloud data to the server and process the server’s response to generate the output label file.

---

## Discussion

### Known Limitations
- Current solution is tailored for the provided dataset and might require tuning for other datasets.
- Clustering implementation is simplified for demonstration purposes and might not be optimal.

### Production Recommendations
- Add authentication to the gRPC service for secure communication.
- Optimize clustering logic for scalability and performance.
- Include automated tests to ensure reliability and accuracy.

### Clustering Approach
- Clustering is performed by classifying points based on the Z-axis value:
  - **Dura**: Points above a specified maximum percentile.
  - **Cortical Surface**: Points below a specified minimum percentile.
  - **Unknown**: Points between the two thresholds.

---

## Additional Resources

- Use the provided `visualize.py` script in the `help` folder to visualize the 3D OCT scans.
- For more information on the `.PCD` file format, visit the [Point Cloud Library documentation](https://pointclouds.org/documentation/tutorials/pcd_file_format.html).

---

## How to Run

### Server
1. Build and run the server using the provided `Dockerfile`.
2. Ensure the server is running and listening on the correct port (default: `50051`).

### Client
1. Run the `client-setup.sh` script to prepare the client.
2. Execute the client using the following command:
    `./client --host <server_host> --port <server_port> --input <input_file> --output <output_file>`



---

## Transparency
- If language models (like ChatGPT) were used in the implementation, the chat log has been included as part of the submission.

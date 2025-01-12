
# Visualization Debugging Project

This directory contains a lightweight Dockerized project to help visualize `.PCD` files and their corresponding labels. The purpose of this project is to debug and analyze point cloud data easily.

## Directory Structure

```
help/
├── Dockerfile             # Dockerfile to set up the environment
├── docker-compose.yml     # Docker Compose file to run the visualization tool
├── visualize.py           # Python script for visualizing point cloud data
├── requirements.txt       # Python dependencies for the project
├── run.sh                 # Bash script to run the project
```

## How to Use

### Prerequisites

- Docker and Docker Compose installed on your machine.
- `.PCD` and `.labels` files are placed in the `data/` directory.

### Building and Running

#### Default Files

To visualize the default `oct-1-small.pcd` file and its corresponding labels:

```bash
./run.sh
```

#### Custom Files

To specify custom `.PCD` and `.labels` files, use the following command:

```bash
./run.sh --pcd <path_to_pcd_file> --labels <path_to_labels_file>
```

Example:

```bash
./run.sh --pcd ../data/oct-2-small.pcd --labels ../data/oct-2-small.labels
```

## Environment Variables

The following environment variables are automatically set by the `run.sh` script:

- **PATH_TO_PCD**: Path to the `.PCD` file to visualize.
- **PATH_TO_LABELS**: Path to the `.labels` file for cluster visualization.

These are passed to the container and used by the `visualize.py` script.

## Visualization Script (`visualize.py`)

The `visualize.py` script provides a way to render 3D point cloud data with clustering labels. Points are colored based on their assigned cluster:

- **Light Blue**: Unknown points.
- **Dark Blue**: Cortical Surface points.
- **Brown**: Dura points.

### Script Usage

```bash
python3 visualize.py --path-to-pcd <path_to_pcd_file> --labels <path_to_labels_file>
```

## Example Files

The following example files are provided in the `data/` directory:

- **Point Cloud Files**: `.pcd` files containing the 3D point cloud data.
  - `oct-1-small.pcd`
  - `oct-2-small.pcd`
  - `oct-3-small.pcd`

- **Cluster Labels**: `.labels` files containing integer cluster labels for each point in the corresponding `.pcd` file.
  - `oct-1-small.labels`
  - `oct-2-small.labels`
  - `oct-3-small.labels`

## Development Notes

- The `requirements.txt` file includes dependencies such as `numpy` and `open3d`.
- The `Dockerfile` installs all required dependencies and sets up the visualization script.
- The `docker-compose.yml` configures the container to use the provided data files.

## Troubleshooting

- Ensure the correct paths to `.PCD` and `.labels` files are provided if using custom files.
- Verify Docker is installed and running correctly on your system.
- Use `docker-compose logs` to view any runtime errors in the container.

## Future Enhancements

- Add support for dynamic clustering methods to be visualized.
- Extend visualization options (e.g., zoom, pan, and save to image).

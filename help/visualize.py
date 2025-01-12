import open3d as o3d
import numpy as np
import argparse

# Function to load point cloud and labels
def load_data(pcd_file, labels_file):
    # Read PCD file
    pcd = o3d.io.read_point_cloud(pcd_file)
    points = np.asarray(pcd.points)

    # Read Labels file
    labels = np.loadtxt(labels_file, dtype=int)

    return points, labels

# Function to count points by label
def count_labels(labels):
    unique, counts = np.unique(labels, return_counts=True)
    label_counts = dict(zip(unique, counts))
    return label_counts

# Visualize point cloud
def visualize(points, labels):
    colors = np.zeros((len(points), 3))  # Initialize color array

    # Assign colors based on label
    colors[labels == 0] = [0.0, 0.0, 1.0]  # Blue for label 0
    colors[labels == 1] = [0.0, 1.0, 0.0]  # Green for label 1
    colors[labels == 2] = [1.0, 0.0, 0.0]  # Red for label 2

    # Visualize point cloud
    pcd = o3d.geometry.PointCloud()
    pcd.points = o3d.utility.Vector3dVector(points)
    pcd.colors = o3d.utility.Vector3dVector(colors)
    o3d.visualization.draw_geometries([pcd])

# Main function to handle command-line arguments
def main():
    # Parse command-line arguments
    parser = argparse.ArgumentParser(description="Visualize Point Cloud with Labels")
    parser.add_argument('-i', '--pcd', type=str, required=True, help="Path to the .pcd file")
    parser.add_argument('-l', '--labels', type=str, required=True, help="Path to the .labels file")
    
    args = parser.parse_args()

    # Load and analyze data
    points, labels = load_data(args.pcd, args.labels)

    # Count and print labels
    label_counts = count_labels(labels)
    print("Label counts:", label_counts)

    # Visualize the point cloud
    visualize(points, labels)

if __name__ == "__main__":
    main()

import open3d as o3d
import numpy as np

# Load point cloud and labels
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

    # Color points based on labels
    colors[labels == 0] = [0.0, 0.0, 1.0]  # Blue
    colors[labels == 1] = [0.0, 1.0, 0.0]  # Green
    colors[labels == 2] = [1.0, 0.0, 0.0]  # Red

    # Visualize point cloud
    pcd = o3d.geometry.PointCloud()
    pcd.points = o3d.utility.Vector3dVector(points)
    pcd.colors = o3d.utility.Vector3dVector(colors)
    o3d.visualization.draw_geometries([pcd])

# Load and analyze data
pcd_file = 'data/oct-2-small.pcd'
labels_file = 'data/oct-2-small.labels'
points, labels = load_data(pcd_file, labels_file)

# Count and print labels
label_counts = count_labels(labels)
print("Label counts:", label_counts)

# Visualize the point cloud
visualize(points, labels)

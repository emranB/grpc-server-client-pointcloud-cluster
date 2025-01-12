# Use Python 3.9-slim as the base image
FROM python:3.9-slim

# Set working directory inside the container
WORKDIR /app

# Copy the requirements file into the image
COPY requirements.txt /app/requirements.txt

# Install dependencies (inside the Docker container)
RUN apt-get update && apt-get install -y \
    libgomp1 libx11-6 libgl1-mesa-glx libgl1 \
    && rm -rf /var/lib/apt/lists/* \
    && pip install --no-cache-dir -r /app/requirements.txt

# Copy the visualize.py script into the container
COPY visualize.py /app/visualize.py

# Copy the entire 'data' directory into the container's /app/data directory
COPY data /app/data

# Copy entrypoint.sh into the container and make it executable
COPY entrypoint.sh /app/entrypoint.sh
RUN chmod +x /app/entrypoint.sh

# Set entrypoint to the entrypoint.sh script
ENTRYPOINT ["/app/entrypoint.sh"]

# Default CMD will just call entrypoint.sh (no need for other parameters here)
CMD []

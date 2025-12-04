#!/bin/bash
# Setup script for screenshot capture utility

echo "Setting up screenshot capture utility..."

# Check if Python 3 is installed
if ! command -v python3 &> /dev/null; then
    echo "Error: Python 3 is not installed"
    exit 1
fi

echo "Python 3 found: $(python3 --version)"

# Install required packages
echo "Installing required Python packages..."
pip3 install -r requirements.txt

# Create screenshots directory
echo "Creating screenshots directory..."
mkdir -p screenshots

# Make the script executable
chmod +x capture_screen.py

echo ""
echo "Setup complete!"
echo ""
echo "Usage:"
echo "  python3 capture_screen.py --port /dev/ttyUSB0"
echo "  python3 capture_screen.py --port COM3  (Windows)"
echo ""
echo "For more options, run:"
echo "  python3 capture_screen.py --help"

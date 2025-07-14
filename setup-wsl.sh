#!/bin/bash

# Thank you LLMs for such scripts


echo "3D Graphics Engine - WSL Environment Setup"
echo "=========================================="
echo ""

# Check if we're in WSL
if ! grep -qEi "(microsoft|wsl)" /proc/version &> /dev/null; then
    echo "This script is designed for WSL (Windows Subsystem for Linux)"
    echo "Please run this in a WSL environment."
    exit 1
fi

echo "Detected WSL environment - proceeding with setup..."
echo ""

# Update package list
echo "Updating package list..."
sudo apt update

# Install build tools
echo "Installing build tools..."
sudo apt install -y build-essential cmake git pkg-config

# Install graphics and X11 development libraries
echo "Installing graphics and X11 development libraries..."
sudo apt install -y \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libx11-dev \
    libxext-dev \
    libxrandr-dev \
    libxi-dev \
    libxcursor-dev \
    libxinerama-dev \
    libxmu-dev \
    libxpm-dev

# Check if DISPLAY is set
echo "Checking X11 configuration..."
if [ -z "$DISPLAY" ]; then
    echo ""
    echo "Setting up DISPLAY environment variable..."
    
    # Detect WSL version
    if grep -q "microsoft" /proc/version; then
        # WSL1
        echo "Detected WSL1"
        echo "export DISPLAY=:0.0" >> ~/.bashrc
        export DISPLAY=:0.0
    else
        # WSL2
        echo "Detected WSL2"
        echo "export DISPLAY=\$(cat /etc/resolv.conf | grep nameserver | awk '{print \$2}'):0.0" >> ~/.bashrc
        export DISPLAY=$(cat /etc/resolv.conf | grep nameserver | awk '{print $2}'):0.0
    fi
    
    echo "DISPLAY variable configured and added to ~/.bashrc"
else
    echo "DISPLAY is already set to: $DISPLAY"
fi

# Check CPU capabilities
echo ""
echo "Checking CPU capabilities for SIMD optimization..."
if grep -q avx2 /proc/cpuinfo; then
    echo "✓ AVX2 support detected"
else
    echo "⚠ AVX2 not detected - performance may be reduced"
fi

if grep -q fma /proc/cpuinfo; then
    echo "✓ FMA support detected"
else
    echo "⚠ FMA not detected - performance may be reduced"
fi

# Test X11 connection
echo ""
echo "Testing X11 connection..."
if command -v xeyes &> /dev/null; then
    echo "X11 utilities already installed"
else
    echo "Installing X11 utilities for testing..."
    sudo apt install -y x11-apps
fi

echo ""
echo "Testing X server connection..."
if timeout 5 xset q &>/dev/null; then
    echo "✓ X server connection successful!"
else
    echo "⚠ Cannot connect to X server"
    echo ""
    echo "Please ensure you have an X server running on Windows:"
    echo "1. Install VcXsrv from: https://sourceforge.net/projects/vcxsrv/"
    echo "2. Start VcXsrv with default settings"
    echo "3. Make sure 'Disable access control' is checked"
    echo "4. Restart this script or run: source ~/.bashrc"
fi

echo ""
echo "WSL environment setup complete!"
echo ""
echo "Next steps:"
echo "1. Restart your WSL terminal or run: source ~/.bashrc"
echo "2. Ensure your X server (VcXsrv) is running on Windows"
echo "3. Build the project with: ./build.sh"
echo ""
echo "Troubleshooting:"
echo "- If you get 'cannot connect to display' errors:"
echo "  - Make sure VcXsrv is running on Windows"
echo "  - Check that DISPLAY is set: echo \$DISPLAY"
echo "  - Try: export DISPLAY=:0.0"
echo ""
echo "- For WSL2 networking issues:"
echo "  - Windows Firewall might block connections"
echo "  - Try disabling Windows Firewall temporarily"
echo "  - Or configure firewall rules for VcXsrv" 

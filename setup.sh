#!/bin/bash
# Setup script for eCall MSD Encoder/Decoder (C/C++ version)

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
echo "Setting up eCall MSD Encoder/Decoder..."
echo "Project directory: $SCRIPT_DIR"

# Check for required tools
echo ""
echo "Checking dependencies..."

# Check for CMake
if ! command -v cmake &> /dev/null; then
    echo "❌ CMake not found. Installing..."
    if [[ "$OSTYPE" == "darwin"* ]]; then
        brew install cmake
    else
        sudo apt-get install -y cmake
    fi
else
    echo "✓ CMake found: $(cmake --version | head -1)"
fi

# Check for asn1c
if ! command -v asn1c &> /dev/null; then
    echo "❌ asn1c not found. Installing..."
    if [[ "$OSTYPE" == "darwin"* ]]; then
        brew install asn1c
    else
        sudo apt-get install -y asn1c
    fi
else
    echo "✓ asn1c found: $(asn1c --version 2>&1 | head -1)"
fi

# Check for C++ compiler
if ! command -v clang++ &> /dev/null && ! command -v g++ &> /dev/null; then
    echo "❌ C++ compiler not found. Installing..."
    if [[ "$OSTYPE" == "darwin"* ]]; then
        xcode-select --install
    else
        sudo apt-get install -y build-essential
    fi
else
    if command -v clang++ &> /dev/null; then
        echo "✓ Compiler found: $(clang++ --version | head -1)"
    else
        echo "✓ Compiler found: $(g++ --version | head -1)"
    fi
fi

# Create build directory
echo ""
echo "Creating build directory..."
if [ -d "$SCRIPT_DIR/build" ]; then
    echo "Build directory already exists"
else
    mkdir -p "$SCRIPT_DIR/build"
    echo "✓ Build directory created"
fi

# Build the project
echo ""
echo "Building project..."
cd "$SCRIPT_DIR/build"

cmake .. -DCMAKE_BUILD_TYPE=Release
make

echo ""
echo "✓ Build completed successfully!"
echo ""
echo "Executables:"
echo "  • Main application: $SCRIPT_DIR/build/ecall-msd-tool"
echo "  • Test suite:       $SCRIPT_DIR/build/ecall-msd-test"
echo ""
echo "Quick start:"
echo "  $SCRIPT_DIR/build/ecall-msd-tool --help"
echo "  $SCRIPT_DIR/build/ecall-msd-tool test"
echo ""
echo "Next steps:"
echo "  1. Read BUILD_GUIDE.md for detailed usage"
echo "  2. Try: ./ecall-msd-tool test"
echo "  3. Try: ./ecall-msd-tool encode --help"
echo ""
echo "Setup complete! ✓"

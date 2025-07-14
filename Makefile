.PHONY: all build run clean configure

# Default target
all: build

# Configure the project with CMake (only when needed)
build/Makefile:
	@echo "Configuring project with CMake..."
	@mkdir -p build
	@cd build && cmake ..

# Manual configure target
configure: build/Makefile

# Build the project
build: build/Makefile
	@echo "Building 3d_engine target..."
	cmake --build build --target 3d_engine

# Run the binary
run: build
	@echo "Running 3d_engine..."
	@cd build/bin && ./3d_engine

# Clean build directory
clean:
	@echo "Cleaning build directory..."
	@rm -rf build

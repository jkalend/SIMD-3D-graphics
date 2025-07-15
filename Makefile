.PHONY: all build run clean configure

all: build

build/Makefile:
	@echo "Configuring project with CMake..."
	@mkdir -p build
	@cd build && cmake ..

configure: build/Makefile

build: build/Makefile
	@echo "Building 3d_engine target..."
	cmake --build build --target 3d_engine

run: build
	@echo "Running 3d_engine..."
	@cd build/bin && ./3d_engine

clean:
	@echo "Cleaning build directory..."
	@rm -rf build

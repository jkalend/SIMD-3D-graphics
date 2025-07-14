cmake_minimum_required(VERSION 3.15)
project(3DEngine VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set default build type to Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# GCC/Clang compiler options (WSL/Linux)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -mavx2 -mfma -march=native")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -mavx2 -mfma -DDEBUG")

# Find required packages
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)

# Find X11 and related libraries
find_package(X11 REQUIRED)
pkg_check_modules(XEXT REQUIRED xext)
pkg_check_modules(XRANDR REQUIRED xrandr)
pkg_check_modules(XI REQUIRED xi)
pkg_check_modules(XCURSOR REQUIRED xcursor)
pkg_check_modules(XINERAMA REQUIRED xinerama)

# Optional: Find additional graphics libraries
find_package(Threads REQUIRED)

# Include directories
include_directories(include)

# Source files
set(MATH_SOURCES
    src/math/vector3.cpp
    src/math/matrix4.cpp
)

set(GRAPHICS_SOURCES
    src/graphics/renderer.cpp
    src/graphics/mesh.cpp
    src/graphics/camera.cpp
)

set(MAIN_SOURCES
    src/main.cpp
)

# Create executable
add_executable(3d_engine
    ${MAIN_SOURCES}
    ${MATH_SOURCES}
    ${GRAPHICS_SOURCES}
)

# Link libraries for Linux/WSL
target_link_libraries(3d_engine
    ${OPENGL_LIBRARIES}
    ${X11_LIBRARIES}
    ${XEXT_LIBRARIES}
    ${XRANDR_LIBRARIES}
    ${XI_LIBRARIES}
    ${XCURSOR_LIBRARIES}
    ${XINERAMA_LIBRARIES}
    Threads::Threads
    m  # Math library
)

# Include directories for X11 libraries
target_include_directories(3d_engine PRIVATE
    ${X11_INCLUDE_DIR}
    ${XEXT_INCLUDE_DIRS}
    ${XRANDR_INCLUDE_DIRS}
    ${XI_INCLUDE_DIRS}
    ${XCURSOR_INCLUDE_DIRS}
    ${XINERAMA_INCLUDE_DIRS}
)

# Set output directories
set_target_properties(3d_engine PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin
)

# Print build information
message(STATUS "Building for WSL/Linux")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ Flags: ${CMAKE_CXX_FLAGS}")
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Release Flags: ${CMAKE_CXX_FLAGS_RELEASE}")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug Flags: ${CMAKE_CXX_FLAGS_DEBUG}")
endif()

# Check for SIMD support
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-mavx2" COMPILER_SUPPORTS_AVX2)
check_cxx_compiler_flag("-mfma" COMPILER_SUPPORTS_FMA)

if(COMPILER_SUPPORTS_AVX2)
    message(STATUS "AVX2 support: YES")
else()
    message(WARNING "AVX2 support: NO - Performance may be reduced")
endif()

if(COMPILER_SUPPORTS_FMA)
    message(STATUS "FMA support: YES")
else()
    message(WARNING "FMA support: NO - Performance may be reduced")
endif()

# Create custom targets for convenience
add_custom_target(run
    COMMAND $<TARGET_FILE:3d_engine>
    DEPENDS 3d_engine
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    COMMENT "Running 3D Engine"
)

add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMENT "Cleaning all build files"
)

# Install target
install(TARGETS 3d_engine
    RUNTIME DESTINATION bin
) 

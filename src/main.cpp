#include "../include/math/vector3.h"
#include "../include/math/matrix4.h"
#include "../include/graphics/renderer.h"
#include "../include/graphics/mesh.h"
#include "../include/graphics/camera.h"
#include <iostream>
#include <chrono>
#include <cmath>

int main() {
    std::cout << "3D Graphics Engine with SIMD Operations" << std::endl;
    std::cout << "========================================" << std::endl;
    
    Renderer renderer;
    if (!renderer.initialize(1280, 720, "3D Engine - SIMD Demo")) {
        std::cerr << "Failed to initialize renderer!" << std::endl;
        return -1;
    }
    
    Camera camera;
    camera.set_perspective(60.0f * M_PI / 180.0f, 1280.0f / 720.0f, 1.0f, 100.0f);
    
    renderer.add_light(Light(Vector3(5, 5, 5), Vector3(1, 1, 1), 1.0f));
    renderer.add_light(Light(Vector3(-5, 5, -5), Vector3(0.8f, 0.8f, 1.0f), 0.7f));
    
    Mesh cube = Mesh::create_cube(2.0f);
    
    std::cout << "Created meshes:" << std::endl;
    std::cout << "  Cube: " << cube.vertex_count() << " vertices, " << cube.triangle_count() << " triangles" << std::endl;
    
    // Camera orbit parameters
    const float camera_distance = 7.0f;  // Distance from cube center
    const float camera_height = 3.0f;    // Height above the cube
    Vector3 look_target;         // Always look at cube center
    
    auto start_time = std::chrono::high_resolution_clock::now();
    float total_time = 0.0f;
    int frame_count = 0;
    
    std::cout << "\nStarting render loop..." << std::endl;
    std::cout << "Controls: ESC to exit" << std::endl;
    std::cout << "Camera orbiting at half cube rotation speed..." << std::endl;
    
    while (!renderer.should_close()) {
        auto current_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(current_time - start_time);
        total_time = duration.count() / 1000000.0f;
        
        renderer.poll_events();
        renderer.begin_frame();
        renderer.clear(Vector3(0.1f, 0.2f, 0.3f));

        float camera_rotation = total_time * 45.0f;  // 45 degrees per second
        float camera_rad = camera_rotation * M_PI / 180.0f;
        
        // Orbit on an angled circle (30 degrees tilt)
        const float orbit_tilt = 30.0f * M_PI / 180.0f;  // 30 degree tilt
        
        Vector3 camera_position(
            camera_distance * std::cos(camera_rad),
            camera_height + camera_distance * std::sin(camera_rad) * std::sin(orbit_tilt),
            camera_distance * std::sin(camera_rad) * std::cos(orbit_tilt)
        );
        
        camera.set_position(camera_position);
        camera.look_at(look_target);
        renderer.set_camera(camera);

        float cube_rotation = total_time * 90.0f;
        Matrix4 cube_transform = Matrix4::rotation_y(cube_rotation * M_PI / 180.0f);
        renderer.draw_mesh(cube, cube_transform);
        renderer.draw_mesh_outline(cube, cube_transform, Vector3(0, 0, 0));

        // Draw coordinate axes
        renderer.draw_line(Vector3(-2, 0, 0), Vector3(2, 0, 0), Vector3(1, 0, 0));
        renderer.draw_line(Vector3(0, -2, 0), Vector3(0, 2, 0), Vector3(0, 1, 0));
        renderer.draw_line(Vector3(0, 0, -2), Vector3(0, 0, 2), Vector3(0, 0, 1));
        
        renderer.end_frame();
        
        frame_count++;
        
        if (frame_count % 60 == 0) {
            float fps = frame_count / total_time;
            std::cout << "FPS: " << static_cast<int>(fps) << " | Time: " << total_time << "s" << std::endl;
        }
    }
    
    std::cout << "\nShutting down..." << std::endl;
    std::cout << "Total frames rendered: " << frame_count << std::endl;
    std::cout << "Average FPS: " << static_cast<int>(frame_count / total_time) << std::endl;
    
    return 0;
} 

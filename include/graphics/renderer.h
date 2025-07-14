#pragma once

#include "../math/vector3.h"
#include "../math/matrix4.h"
#include "mesh.h"
#include "camera.h"
#include <vector>

// Linux/WSL includes
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>

struct Light {
    Vector3 position;
    Vector3 color;
    float intensity;
    
    Light(const Vector3& pos = Vector3(0, 10, 0), 
          const Vector3& col = Vector3(1, 1, 1), 
          float intens = 1.0f)
        : position(pos), color(col), intensity(intens) {}
};

class Renderer {
public:
    Renderer();
    ~Renderer();
    
    bool initialize(int width, int height, const char* title);
    void shutdown();
    
    void begin_frame();
    void end_frame();
    void clear(const Vector3& color = Vector3(0.2f, 0.3f, 0.4f));
    
    void draw_mesh(const Mesh& mesh, const Matrix4& transform);
    void draw_wireframe_mesh(const Mesh& mesh, const Matrix4& model_matrix);
    void draw_line(const Vector3& start, const Vector3& end, const Vector3& color = Vector3(1, 1, 1));
    void draw_mesh_outline(const Mesh& mesh, const Matrix4& transform, const Vector3& color);
    
    void set_camera(const Camera& camera) { _camera = camera; }
    void add_light(const Light& light) { _lights.push_back(light); }
    void clear_lights() { _lights.clear(); }
    
    bool should_close() const;
    void poll_events();
    void swap_buffers();
    
    int width() const { return _width; }
    int height() const { return _height; }
    
private:
    void setup_matrices();
    Vector3 calculate_lighting(const Vector3& position, const Vector3& normal, const Vector3& color);
    bool setup_opengl();
    
    int _width, _height;
    Camera _camera;
    std::vector<Light> _lights;
    
    // X11/Linux specific handles
    Display* _display;
    Window _window;
    GLXContext _glx_context;
    XVisualInfo* _visual_info;
    Colormap _colormap;
    XSetWindowAttributes _window_attributes;
    
    bool _initialized;
    bool _should_close;
}; 

#include "../../include/graphics/renderer.h"
#include <iostream>
#include <cstring>

Renderer::Renderer()
    : _width(0)
    , _height(0)
    , _display(nullptr)
    , _window(0)
    , _glx_context(nullptr)
    , _visual_info(nullptr)
    , _colormap(0)
    , _initialized(false)
    , _should_close(false) {
}

Renderer::~Renderer() {
    shutdown();
}

bool Renderer::initialize(int width, int height, const char* title) {
    _width = width;
    _height = height;
    
    _display = XOpenDisplay(nullptr);
    if (!_display) {
        std::cerr << "Failed to open X11 display. Make sure DISPLAY is set correctly." << std::endl;
        std::cerr << "For WSL, you may need to install an X server like VcXsrv or X410." << std::endl;
        return false;
    }
    
    int glx_major, glx_minor;
    if (!glXQueryVersion(_display, &glx_major, &glx_minor)) {
        std::cerr << "GLX extension not available" << std::endl;
        XCloseDisplay(_display);
        return false;
    }
    
    std::cout << "GLX version: " << glx_major << "." << glx_minor << std::endl;
    
    int attributes[] = {
        GLX_RGBA,
        GLX_DEPTH_SIZE, 24,
        GLX_DOUBLEBUFFER,
        None
    };
    
    _visual_info = glXChooseVisual(_display, DefaultScreen(_display), attributes);
    if (!_visual_info) {
        std::cerr << "Failed to choose appropriate visual" << std::endl;
        XCloseDisplay(_display);
        return false;
    }
    
    _colormap = XCreateColormap(_display, RootWindow(_display, _visual_info->screen),
                               _visual_info->visual, AllocNone);
    
    _window_attributes.colormap = _colormap;
    _window_attributes.event_mask = ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask;
    
    _window = XCreateWindow(_display, RootWindow(_display, _visual_info->screen),
                           0, 0, width, height, 0, _visual_info->depth, InputOutput,
                           _visual_info->visual, CWColormap | CWEventMask, &_window_attributes);
    
    if (!_window) {
        std::cerr << "Failed to create X11 window" << std::endl;
        XFreeColormap(_display, _colormap);
        XFree(_visual_info);
        XCloseDisplay(_display);
        return false;
    }
    
    XStoreName(_display, _window, title);
    
    _glx_context = glXCreateContext(_display, _visual_info, nullptr, GL_TRUE);
    if (!_glx_context) {
        std::cerr << "Failed to create GLX context" << std::endl;
        XDestroyWindow(_display, _window);
        XFreeColormap(_display, _colormap);
        XFree(_visual_info);
        XCloseDisplay(_display);
        return false;
    }
    
    if (!glXMakeCurrent(_display, _window, _glx_context)) {
        std::cerr << "Failed to make GLX context current" << std::endl;
        glXDestroyContext(_display, _glx_context);
        XDestroyWindow(_display, _window);
        XFreeColormap(_display, _colormap);
        XFree(_visual_info);
        XCloseDisplay(_display);
        return false;
    }
    
    XMapWindow(_display, _window);
    XFlush(_display);
    
    if (!setup_opengl()) {
        std::cerr << "Failed to setup OpenGL" << std::endl;
        return false;
    }
    
    _initialized = true;
    std::cout << "Renderer initialized successfully for WSL/Linux" << std::endl;
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
    
    return true;
}

bool Renderer::setup_opengl() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    glDisable(GL_BLEND);
    
    glViewport(0, 0, _width, _height);
    
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error during setup: " << error << std::endl;
        return false;
    }
    
    return true;
}

void Renderer::shutdown() {
    if (!_initialized) return;
    
    if (_glx_context) {
        glXMakeCurrent(_display, None, nullptr);
        glXDestroyContext(_display, _glx_context);
        _glx_context = nullptr;
    }
    
    if (_window) {
        XDestroyWindow(_display, _window);
        _window = 0;
    }
    
    if (_colormap) {
        XFreeColormap(_display, _colormap);
        _colormap = 0;
    }
    
    if (_visual_info) {
        XFree(_visual_info);
        _visual_info = nullptr;
    }
    
    if (_display) {
        XCloseDisplay(_display);
        _display = nullptr;
    }
    
    _initialized = false;
    std::cout << "Renderer shutdown complete" << std::endl;
}

void Renderer::begin_frame() {
    setup_matrices();
}

void Renderer::end_frame() {
    swap_buffers();
}

void Renderer::clear(const Vector3& color) {
    glClearColor(color.x(), color.y(), color.z(), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::draw_mesh(const Mesh& mesh, const Matrix4& model_matrix) {
    const auto& vertices = mesh.vertices();
    const auto& indices = mesh.indices();
    
    if (vertices.empty() || indices.empty()) return;
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    
    Matrix4 model_transposed = model_matrix.transpose();
    glMultMatrixf(model_transposed.data());
    
    // Pass 1: Draw back faces (the "inside" of the cube)
    // We make them slightly darker for visual distinction.
    glCullFace(GL_FRONT);
    glBegin(GL_TRIANGLES);
    for (size_t i = 0; i < indices.size(); ++i) {
        const Vertex& vertex = vertices[indices[i]];
        
        Vector3 world_pos = model_matrix.transform_point(vertex.position);
        Vector3 world_normal = model_matrix.transform_vector(vertex.normal).normalized();
        // Render inside with a darker, ambient-only color
        Vector3 inside_color = vertex.color * 0.15f;
        
        glColor3f(inside_color.x(), inside_color.y(), inside_color.z());
        glNormal3f(world_normal.x(), world_normal.y(), world_normal.z());
        glVertex3f(vertex.position.x(), vertex.position.y(), vertex.position.z());
    }
    glEnd();
    
    // Pass 2: Draw front faces (the "outside") with full lighting
    glCullFace(GL_BACK);
    glBegin(GL_TRIANGLES);
    for (size_t i = 0; i < indices.size(); ++i) {
        const Vertex& vertex = vertices[indices[i]];
        
        // Calculate lighting
        Vector3 world_pos = model_matrix.transform_point(vertex.position);
        Vector3 world_normal = model_matrix.transform_vector(vertex.normal).normalized();
        Vector3 lit_color = calculate_lighting(world_pos, world_normal, vertex.color);
        
        glColor3f(lit_color.x(), lit_color.y(), lit_color.z());
        glNormal3f(world_normal.x(), world_normal.y(), world_normal.z());
        glVertex3f(vertex.position.x(), vertex.position.y(), vertex.position.z());
    }
    glEnd();
    
    glPopMatrix();
}

void Renderer::draw_wireframe_mesh(const Mesh& mesh, const Matrix4& model_matrix) {
    const auto& vertices = mesh.vertices();
    const auto& indices = mesh.indices();
    
    if (vertices.empty() || indices.empty()) return;
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    
    Matrix4 model_transposed = model_matrix.transpose();
    glMultMatrixf(model_transposed.data());
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(1.0f, 1.0f, 1.0f);
    
    glBegin(GL_TRIANGLES);
    for (size_t i = 0; i < indices.size(); ++i) {
        const Vertex& vertex = vertices[indices[i]];
        glVertex3f(vertex.position.x(), vertex.position.y(), vertex.position.z());
    }
    glEnd();
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    glPopMatrix();
}

void Renderer::draw_mesh_outline(const Mesh& mesh, const Matrix4& transform, const Vector3& color) {
    const auto& vertices = mesh.vertices();
    const auto& indices = mesh.indices();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    
    Matrix4 model_transposed = transform.transpose();
    glMultMatrixf(model_transposed.data());
    
    glColor3f(color.x(), color.y(), color.z());
    
    glBegin(GL_LINES);
    for (size_t i = 0; i < indices.size(); i += 3) {
        const Vector3& v0 = vertices[indices[i]].position;
        const Vector3& v1 = vertices[indices[i + 1]].position;
        const Vector3& v2 = vertices[indices[i + 2]].position;

        glVertex3f(v0.x(), v0.y(), v0.z());
        glVertex3f(v1.x(), v1.y(), v1.z());
        
        glVertex3f(v1.x(), v1.y(), v1.z());
        glVertex3f(v2.x(), v2.y(), v2.z());
        
        glVertex3f(v2.x(), v2.y(), v2.z());
        glVertex3f(v0.x(), v0.y(), v0.z());
    }
    glEnd();
    
    glPopMatrix();
}

void Renderer::draw_line(const Vector3& start, const Vector3& end, const Vector3& color) {
    glColor3f(color.x(), color.y(), color.z());
    glBegin(GL_LINES);
    glVertex3f(start.x(), start.y(), start.z());
    glVertex3f(end.x(), end.y(), end.z());
    glEnd();
}

bool Renderer::should_close() const {
    return _should_close;
}

void Renderer::poll_events() {
    XEvent event;
    
    while (XPending(_display)) {
        XNextEvent(_display, &event);
        
        switch (event.type) {
        case KeyPress: {
            KeySym key = XLookupKeysym(&event.xkey, 0);
            if (key == XK_Escape || key == XK_q) {
                _should_close = true;
            }
            break;
        }
        case ClientMessage:
        case DestroyNotify:
            _should_close = true;
            break;
        case ConfigureNotify:
            if (event.xconfigure.width != _width || event.xconfigure.height != _height) {
                _width = event.xconfigure.width;
                _height = event.xconfigure.height;
                glViewport(0, 0, _width, _height);
                
                Camera updated_camera = _camera;
                updated_camera.set_perspective(
                    updated_camera.fov(),
                    static_cast<float>(_width) / static_cast<float>(_height),
                    updated_camera.near_plane(),
                    updated_camera.far_plane()
                );
                _camera = updated_camera;
            }
            break;
        }
    }
}

void Renderer::swap_buffers() {
    glXSwapBuffers(_display, _window);
}

void Renderer::setup_matrices() {
    glMatrixMode(GL_PROJECTION);
    // Transpose for OpenGL column-major format
    Matrix4 proj_transposed = _camera.projection_matrix().transpose();
    glLoadMatrixf(proj_transposed.data());
    
    glMatrixMode(GL_MODELVIEW);
    // Transpose for OpenGL column-major format  
    Matrix4 view_transposed = _camera.view_matrix().transpose();
    glLoadMatrixf(view_transposed.data());
}

Vector3 Renderer::calculate_lighting(const Vector3& position, const Vector3& normal, const Vector3& color) {
    Vector3 final_color = color * 0.1f; // Ambient lighting
    
    for (const auto& light : _lights) {
        Vector3 light_dir = (light.position - position).normalized();
        float dot_product = std::max(0.0f, normal.dot(light_dir));
        
        Vector3 diffuse = light.color * (dot_product * light.intensity);
        final_color = final_color + (color * diffuse);
    }
    
    // Clamp to [0, 1]
    final_color.set_x(std::min(1.0f, std::max(0.0f, final_color.x())));
    final_color.set_y(std::min(1.0f, std::max(0.0f, final_color.y())));
    final_color.set_z(std::min(1.0f, std::max(0.0f, final_color.z())));
    
    return final_color;
} 

#pragma once

#include "../math/vector3.h"
#include "../math/matrix4.h"

class Camera {
public:
    Camera();
    Camera(const Vector3& position, const Vector3& target, const Vector3& up = Vector3(0, 1, 0));
    
    void look_at(const Vector3& target);
    void move(const Vector3& offset);
    void rotate(float yaw, float pitch);
    
    Matrix4 view_matrix() const;
    Matrix4 projection_matrix() const;
    Matrix4 view_projection_matrix() const;
    
    void set_position(const Vector3& position) { _position = position; _view_dirty = true; }
    void set_target(const Vector3& target) { _target = target; _view_dirty = true; }
    void set_up(const Vector3& up) { _up = up; _view_dirty = true; }
    
    void set_perspective(float fov, float aspect, float near, float far);
    void set_orthographic(float left, float right, float bottom, float top, float near, float far);
    
    const Vector3& position() const { return _position; }
    const Vector3& target() const { return _target; }
    const Vector3& up() const { return _up; }
    Vector3 forward() const;
    Vector3 right() const;
    
    float fov() const { return _fov; }
    float aspect_ratio() const { return _aspect_ratio; }
    float near_plane() const { return _near_plane; }
    float far_plane() const { return _far_plane; }
    
private:
    void update_view_matrix() const;
    void update_projection_matrix() const;
    
    Vector3 _position;
    Vector3 _target;
    Vector3 _up;
    
    float _fov;
    float _aspect_ratio;
    float _near_plane;
    float _far_plane;
    
    mutable Matrix4 _view_matrix;
    mutable Matrix4 _projection_matrix;
    mutable bool _view_dirty;
    mutable bool _projection_dirty;
}; 

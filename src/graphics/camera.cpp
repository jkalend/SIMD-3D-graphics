#include "../../include/graphics/camera.h"
#include <cmath>

Camera::Camera() 
    : _position(0, 0, 5)
    , _target(0, 0, 0)
    , _up(0, 1, 0)
    , _fov(45.0f * M_PI / 180.0f)
    , _aspect_ratio(16.0f / 9.0f)
    , _near_plane(0.1f)
    , _far_plane(100.0f)
    , _view_dirty(true)
    , _projection_dirty(true) {
}

Camera::Camera(const Vector3& position, const Vector3& target, const Vector3& up)
    : _position(position)
    , _target(target)
    , _up(up)
    , _fov(45.0f * M_PI / 180.0f)
    , _aspect_ratio(16.0f / 9.0f)
    , _near_plane(0.1f)
    , _far_plane(100.0f)
    , _view_dirty(true)
    , _projection_dirty(true) {
}

void Camera::look_at(const Vector3& target) {
    _target = target;
    _view_dirty = true;
}

void Camera::move(const Vector3& offset) {
    _position = _position + offset;
    _target = _target + offset;
    _view_dirty = true;
}

void Camera::rotate(float yaw, float pitch) {
    Vector3 forward = (_target - _position).normalized();
    Vector3 right = forward.cross(_up).normalized();
    
    Matrix4 yaw_rotation = Matrix4::rotation_y(yaw);
    forward = yaw_rotation.transform_vector(forward);
    
    Matrix4 pitch_rotation = Matrix4::rotation(right, pitch);
    forward = pitch_rotation.transform_vector(forward);
    
    _target = _position + forward;
    _view_dirty = true;
}

Matrix4 Camera::view_matrix() const {
    if (_view_dirty) {
        update_view_matrix();
    }
    return _view_matrix;
}

Matrix4 Camera::projection_matrix() const {
    if (_projection_dirty) {
        update_projection_matrix();
    }
    return _projection_matrix;
}

Matrix4 Camera::view_projection_matrix() const {
    return projection_matrix() * view_matrix();
}

void Camera::set_perspective(float fov, float aspect, float near, float far) {
    _fov = fov;
    _aspect_ratio = aspect;
    _near_plane = near;
    _far_plane = far;
    _projection_dirty = true;
}

[[maybe_unused]]
void Camera::set_orthographic(float /*left*/, float /*right*/, float /*bottom*/, float /*top*/, float near, float far) {
    // For simplicity, this implementation focuses on perspective projection
    // Orthographic projection would be implemented similarly
    _near_plane = near;
    _far_plane = far;
    _projection_dirty = true;
}

Vector3 Camera::forward() const {
    return (_target - _position).normalized();
}

Vector3 Camera::right() const {
    return forward().cross(_up).normalized();
}

void Camera::update_view_matrix() const {
    _view_matrix = Matrix4::look_at(_position, _target, _up);
    _view_dirty = false;
}

void Camera::update_projection_matrix() const {
    _projection_matrix = Matrix4::perspective(_fov, _aspect_ratio, _near_plane, _far_plane);
    _projection_dirty = false;
} 

#pragma once

#include <immintrin.h>
#include "vector3.h"

// Check for AVX support at compile time
#ifndef __AVX__
    #error "AVX support required. Please compile with -mavx or equivalent flag."
#endif

class alignas(16) Matrix4 {
public:
    Matrix4();
    Matrix4(const float* data);
    Matrix4(const Matrix4& other);
    
    Matrix4& operator=(const Matrix4& other);
    
    static Matrix4 identity();
    static Matrix4 zero();
    static Matrix4 translation(const Vector3& translation);
    static Matrix4 rotation_x(float angle);
    static Matrix4 rotation_y(float angle);
    static Matrix4 rotation_z(float angle);
    static Matrix4 rotation(const Vector3& axis, float angle); // Arbitrary axis rotation
    static Matrix4 scale(const Vector3& scale);
    static Matrix4 scale(float uniform_scale);
    static Matrix4 perspective(float fov, float aspect, float near, float far);
    static Matrix4 orthographic(float left, float right, float bottom, float top, float near, float far);
    static Matrix4 look_at(const Vector3& eye, const Vector3& center, const Vector3& up);
    
    Matrix4 operator*(const Matrix4& other) const;
    Matrix4 operator+(const Matrix4& other) const;
    Matrix4 operator-(const Matrix4& other) const;
    Matrix4 operator*(float scalar) const;
    
    Vector3 transform_point(const Vector3& point) const;
    Vector3 transform_vector(const Vector3& vector) const;
    
    float& operator()(int row, int col) { return _data[row * 4 + col]; }
    const float& operator()(int row, int col) const { return _data[row * 4 + col]; }
    
    const float* data() const { return _data; }
    const __m128* rows() const { return _rows; }
    
    Matrix4 transpose() const;
    Matrix4 inverse() const;
    float determinant() const;
    bool is_invertible(float epsilon = 1e-6f) const;
    
    void print() const;
    
    [[maybe_unused]]
    bool decompose(Vector3& translation, Vector3& rotation, Vector3& scale) const;
    
private:
    union {
        __m128 _rows[4]; // Store as 4 SIMD vectors (rows)
        float _data[16];  // Row-major order: [0-3]=row0, [4-7]=row1, etc.
    };
}; 

#pragma once

#include <immintrin.h>
#include <cmath>
#include <iostream>

// Check for AVX support at compile time
#ifndef __AVX__
    #error "AVX support required. Please compile with -mavx or equivalent flag."
#endif

class alignas(16) Vector3 {
public:
    Vector3();
    Vector3(float x, float y, float z);
    Vector3(const __m128& simd_data);
    
    Vector3(const Vector3& other);
    Vector3& operator=(const Vector3& other);
    
    float x() const { return _data[0]; }
    float y() const { return _data[1]; }
    float z() const { return _data[2]; }
    
    void set_x(float x) { _data[0] = x; }
    void set_y(float y) { _data[1] = y; }
    void set_z(float z) { _data[2] = z; }
    
    // AVX-optimized operations
    Vector3 operator+(const Vector3& other) const;
    Vector3 operator-(const Vector3& other) const;
    Vector3 operator*(float scalar) const;
    Vector3 operator*(const Vector3& other) const;
    
    float dot(const Vector3& other) const;
    
    Vector3 cross(const Vector3& other) const;
    
    float length_squared() const;
    float length() const;
    Vector3 normalized() const;
    void normalize();
    
    void print() const;
    
    const __m128& simd_data() const { return _simd; }
    
    static Vector3 zero() { return Vector3(); }
    static Vector3 one() { return Vector3(1.0f, 1.0f, 1.0f); }
    static Vector3 up() { return Vector3(0.0f, 1.0f, 0.0f); }
    static Vector3 right() { return Vector3(1.0f, 0.0f, 0.0f); }
    static Vector3 forward() { return Vector3(0.0f, 0.0f, 1.0f); }
    
private:
    union {
        __m128 _simd;
        float _data[4]; // x, y, z, w (w is padding for SIMD alignment)
    };
};

Vector3 operator*(float scalar, const Vector3& vec);
std::ostream& operator<<(std::ostream& os, const Vector3& vec); 

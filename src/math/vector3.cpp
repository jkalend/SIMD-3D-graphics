#include "../../include/math/vector3.h"
#include <iomanip>

Vector3::Vector3() {
    _simd = _mm_setzero_ps();
}

Vector3::Vector3(float x, float y, float z) {
    _simd = _mm_set_ps(0.0f, z, y, x);
}

Vector3::Vector3(const __m128& simd_data) : _simd(simd_data) {}

Vector3::Vector3(const Vector3& other) : _simd(other._simd) {}

Vector3& Vector3::operator=(const Vector3& other) {
    if (this != &other) {
        _simd = other._simd;
    }
    return *this;
}

Vector3 Vector3::operator+(const Vector3& other) const {
    return Vector3(_mm_add_ps(_simd, other._simd));
}

Vector3 Vector3::operator-(const Vector3& other) const {
    return Vector3(_mm_sub_ps(_simd, other._simd));
}

Vector3 Vector3::operator*(float scalar) const {
    __m128 scalar_vec = _mm_set1_ps(scalar);
    return Vector3(_mm_mul_ps(_simd, scalar_vec));
}

Vector3 Vector3::operator*(const Vector3& other) const {
    return Vector3(_mm_mul_ps(_simd, other._simd));
}

float Vector3::dot(const Vector3& other) const {
    // Use AVX's more efficient approach
    // Since w component is always 0, we can safely use all 4 components
    __m128 mul = _mm_mul_ps(_simd, other._simd);
    
    // Use dp (dot product) intrinsic if available, otherwise use horizontal add
    #ifdef __SSE4_1__
        // Mask 0x71 = 0111 0001: multiply first 3 elements, store result in lowest element
        __m128 result = _mm_dp_ps(mul, _mm_set1_ps(1.0f), 0x71);
        return _mm_cvtss_f32(result);
    #else
        // Fallback to manual horizontal add for older CPUs
        // Add x+y, z+w
        __m128 hadd1 = _mm_hadd_ps(mul, mul);
        // Add (x+y) + (z+w)
        __m128 hadd2 = _mm_hadd_ps(hadd1, hadd1);
        return _mm_cvtss_f32(hadd2);
    #endif
}

Vector3 Vector3::cross(const Vector3& other) const {
    // Standard cross product formula:
    // x = (a.y * b.z) - (a.z * b.y)
    // y = (a.z * b.x) - (a.x * b.z)
    // z = (a.x * b.y) - (a.y * b.x)
    
    // Use SIMD shuffles to align components for parallel multiplication
    __m128 a_yzx = _mm_shuffle_ps(_simd, _simd, _MM_SHUFFLE(3, 0, 2, 1));
    __m128 b_zxy = _mm_shuffle_ps(other._simd, other._simd, _MM_SHUFFLE(3, 1, 0, 2));
    
    __m128 a_zxy = _mm_shuffle_ps(_simd, _simd, _MM_SHUFFLE(3, 1, 0, 2));
    __m128 b_yzx = _mm_shuffle_ps(other._simd, other._simd, _MM_SHUFFLE(3, 0, 2, 1));
    
    #ifdef __FMA__
        // Use FMA for fused multiply-subtract: (a.yzx * b.zxy) - (a.zxy * b.yzx)
        // This is more precise and faster than separate multiply + subtract
        // Using fnmadd: result = (a.yzx * b.zxy) + (-(a.zxy * b.yzx))
        __m128 result = _mm_fnmadd_ps(a_zxy, b_yzx, _mm_mul_ps(a_yzx, b_zxy));
    #else
        // Fallback for CPUs without FMA support
        __m128 term1 = _mm_mul_ps(a_yzx, b_zxy);
        __m128 term2 = _mm_mul_ps(a_zxy, b_yzx);
        __m128 result = _mm_sub_ps(term1, term2);
    #endif
    
    return Vector3(result);
}

float Vector3::length_squared() const {
    return dot(*this);
}

float Vector3::length() const {
    float len_sq = length_squared();
    
    #ifdef __AVX__
        __m128 len_vec = _mm_set_ss(len_sq);
        __m128 result = _mm_sqrt_ss(len_vec);
        return _mm_cvtss_f32(result);
    #else
        return std::sqrt(len_sq);
    #endif
}

Vector3 Vector3::normalized() const {
    float len = length();
    if (len > 1e-8f) { // Use small epsilon for better numerical stability
        float inv_len = 1.0f / len;
        return *this * inv_len;
    }
    return Vector3::zero();
}

void Vector3::normalize() {
    float len = length();
    if (len > 1e-8f) { // Use small epsilon for better numerical stability
        float inv_len = 1.0f / len;
        *this = *this * inv_len;
    } else {
        *this = Vector3::zero();
    }
}

void Vector3::print() const {
    std::cout << std::fixed << std::setprecision(3) 
              << "(" << x() << ", " << y() << ", " << z() << ")" << std::endl;
}

Vector3 operator*(float scalar, const Vector3& vec) {
    return vec * scalar;
}

std::ostream& operator<<(std::ostream& os, const Vector3& vec) {
    os << "(" << vec.x() << ", " << vec.y() << ", " << vec.z() << ")";
    return os;
} 

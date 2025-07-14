#include "../../include/math/matrix4.h"
#include <cstring>
#include <iomanip>
#include <iostream>
#include <cmath>

// Pre-computed identity for even faster access
static const Matrix4 IDENTITY_MATRIX = Matrix4();

// Constructors
Matrix4::Matrix4() {
    // Initialize to identity matrix using AVX
    _rows[0] = _mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f);
    _rows[1] = _mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f);
    _rows[2] = _mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f);
    _rows[3] = _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f);
}

Matrix4::Matrix4(const float* data) {
    // Load data using SIMD
    _rows[0] = _mm_loadu_ps(&data[0]);
    _rows[1] = _mm_loadu_ps(&data[4]);
    _rows[2] = _mm_loadu_ps(&data[8]);
    _rows[3] = _mm_loadu_ps(&data[12]);
}

Matrix4::Matrix4(const Matrix4& other) {
    _rows[0] = other._rows[0];
    _rows[1] = other._rows[1];
    _rows[2] = other._rows[2];
    _rows[3] = other._rows[3];
}

Matrix4& Matrix4::operator=(const Matrix4& other) {
    if (this != &other) {
        _rows[0] = other._rows[0];
        _rows[1] = other._rows[1];
        _rows[2] = other._rows[2];
        _rows[3] = other._rows[3];
    }
    return *this;
}

// Static factory methods
Matrix4 Matrix4::identity() {
    return IDENTITY_MATRIX;
}

Matrix4 Matrix4::zero() {
    Matrix4 result;
    __m128 zero_vec = _mm_setzero_ps();
    result._rows[0] = zero_vec;
    result._rows[1] = zero_vec;
    result._rows[2] = zero_vec;
    result._rows[3] = zero_vec;
    return result;
}

Matrix4 Matrix4::translation(const Vector3& translation) {
    Matrix4 result;
    result(0, 3) = translation.x();
    result(1, 3) = translation.y();
    result(2, 3) = translation.z();
    return result;
}

Matrix4 Matrix4::rotation_x(float angle) {
    Matrix4 result;
    float cos_a = std::cos(angle);
    float sin_a = std::sin(angle);
    
    result(1, 1) = cos_a;
    result(1, 2) = -sin_a;
    result(2, 1) = sin_a;
    result(2, 2) = cos_a;
    
    return result;
}

Matrix4 Matrix4::rotation_y(float angle) {
    Matrix4 result;
    float cos_a = std::cos(angle);
    float sin_a = std::sin(angle);
    
    result(0, 0) = cos_a;
    result(0, 2) = sin_a;
    result(2, 0) = -sin_a;
    result(2, 2) = cos_a;
    
    return result;
}

Matrix4 Matrix4::rotation_z(float angle) {
    Matrix4 result;
    float cos_a = std::cos(angle);
    float sin_a = std::sin(angle);
    
    result(0, 0) = cos_a;
    result(0, 1) = -sin_a;
    result(1, 0) = sin_a;
    result(1, 1) = cos_a;
    
    return result;
}

Matrix4 Matrix4::rotation(const Vector3& axis, float angle) {
    Vector3 normalized_axis = axis.normalized();
    float cos_a = std::cos(angle);
    float sin_a = std::sin(angle);
    float one_minus_cos = 1.0f - cos_a;
    
    float x = normalized_axis.x();
    float y = normalized_axis.y();
    float z = normalized_axis.z();
    
    Matrix4 result;
    
    // Rodrigues' rotation formula
    result(0, 0) = cos_a + x * x * one_minus_cos;
    result(0, 1) = x * y * one_minus_cos - z * sin_a;
    result(0, 2) = x * z * one_minus_cos + y * sin_a;
    
    result(1, 0) = y * x * one_minus_cos + z * sin_a;
    result(1, 1) = cos_a + y * y * one_minus_cos;
    result(1, 2) = y * z * one_minus_cos - x * sin_a;
    
    result(2, 0) = z * x * one_minus_cos - y * sin_a;
    result(2, 1) = z * y * one_minus_cos + x * sin_a;
    result(2, 2) = cos_a + z * z * one_minus_cos;
    
    return result;
}

Matrix4 Matrix4::scale(const Vector3& scale) {
    Matrix4 result;
    result(0, 0) = scale.x();
    result(1, 1) = scale.y();
    result(2, 2) = scale.z();
    return result;
}

Matrix4 Matrix4::scale(float uniform_scale) {
    return scale(Vector3(uniform_scale, uniform_scale, uniform_scale));
}

Matrix4 Matrix4::perspective(float fov, float aspect, float near, float far) {
    Matrix4 result = zero();
    
    float tan_half_fov = std::tan(fov * 0.5f);
    
    result(0, 0) = 1.0f / (aspect * tan_half_fov);
    result(1, 1) = 1.0f / tan_half_fov;
    result(2, 2) = -(far + near) / (far - near);
    result(2, 3) = -(2.0f * far * near) / (far - near);
    result(3, 2) = -1.0f;
    
    return result;
}

Matrix4 Matrix4::orthographic(float left, float right, float bottom, float top, float near, float far) {
    Matrix4 result = zero();
    
    result(0, 0) = 2.0f / (right - left);
    result(1, 1) = 2.0f / (top - bottom);
    result(2, 2) = -2.0f / (far - near);
    result(0, 3) = -(right + left) / (right - left);
    result(1, 3) = -(top + bottom) / (top - bottom);
    result(2, 3) = -(far + near) / (far - near);
    result(3, 3) = 1.0f;
    
    return result;
}

Matrix4 Matrix4::look_at(const Vector3& eye, const Vector3& center, const Vector3& up) {
    // Classic OpenGL gluLookAt implementation
    Vector3 f = (center - eye).normalized();  // Forward direction (towards target)
    Vector3 s = f.cross(up).normalized();     // Side (right) direction  
    Vector3 u = s.cross(f);                   // Up direction
    
    // Build the view matrix manually - this is the exact gluLookAt formula
    Matrix4 result = identity();
    
    // First 3x3 is the rotation part (inverse of camera orientation)
    result(0, 0) = s.x();   result(0, 1) = s.y();   result(0, 2) = s.z();
    result(1, 0) = u.x();   result(1, 1) = u.y();   result(1, 2) = u.z();
    result(2, 0) = -f.x();  result(2, 1) = -f.y();  result(2, 2) = -f.z();
    
    // Translation part (move world so camera is at origin)
    result(0, 3) = -s.dot(eye);
    result(1, 3) = -u.dot(eye);  
    result(2, 3) = f.dot(eye);
    
    // Bottom row
    result(3, 0) = 0.0f;
    result(3, 1) = 0.0f;
    result(3, 2) = 0.0f;
    result(3, 3) = 1.0f;
    
    return result;
}

// AVX-optimized operations
Matrix4 Matrix4::operator*(const Matrix4& other) const {
    Matrix4 result;
    
    // AVX-optimized matrix multiplication
    for (int i = 0; i < 4; ++i) {
        __m128 row = _rows[i];
        
        // Extract each component of the row and broadcast it
        __m128 x = _mm_shuffle_ps(row, row, _MM_SHUFFLE(0, 0, 0, 0));
        __m128 y = _mm_shuffle_ps(row, row, _MM_SHUFFLE(1, 1, 1, 1));
        __m128 z = _mm_shuffle_ps(row, row, _MM_SHUFFLE(2, 2, 2, 2));
        __m128 w = _mm_shuffle_ps(row, row, _MM_SHUFFLE(3, 3, 3, 3));
        
        // Use FMA if available for better precision and performance
        #ifdef __FMA__
            __m128 result_row = _mm_mul_ps(x, other._rows[0]);
            result_row = _mm_fmadd_ps(y, other._rows[1], result_row);
            result_row = _mm_fmadd_ps(z, other._rows[2], result_row);
            result_row = _mm_fmadd_ps(w, other._rows[3], result_row);
        #else
            // Traditional approach
            __m128 result_row = _mm_add_ps(
                _mm_add_ps(
                    _mm_mul_ps(x, other._rows[0]),
                    _mm_mul_ps(y, other._rows[1])
                ),
                _mm_add_ps(
                    _mm_mul_ps(z, other._rows[2]),
                    _mm_mul_ps(w, other._rows[3])
                )
            );
        #endif
        
        result._rows[i] = result_row;
    }
    
    return result;
}

Matrix4 Matrix4::operator+(const Matrix4& other) const {
    Matrix4 result;
    result._rows[0] = _mm_add_ps(_rows[0], other._rows[0]);
    result._rows[1] = _mm_add_ps(_rows[1], other._rows[1]);
    result._rows[2] = _mm_add_ps(_rows[2], other._rows[2]);
    result._rows[3] = _mm_add_ps(_rows[3], other._rows[3]);
    return result;
}

Matrix4 Matrix4::operator-(const Matrix4& other) const {
    Matrix4 result;
    result._rows[0] = _mm_sub_ps(_rows[0], other._rows[0]);
    result._rows[1] = _mm_sub_ps(_rows[1], other._rows[1]);
    result._rows[2] = _mm_sub_ps(_rows[2], other._rows[2]);
    result._rows[3] = _mm_sub_ps(_rows[3], other._rows[3]);
    return result;
}

Matrix4 Matrix4::operator*(float scalar) const {
    Matrix4 result;
    __m128 scalar_vec = _mm_set1_ps(scalar);
    result._rows[0] = _mm_mul_ps(_rows[0], scalar_vec);
    result._rows[1] = _mm_mul_ps(_rows[1], scalar_vec);
    result._rows[2] = _mm_mul_ps(_rows[2], scalar_vec);
    result._rows[3] = _mm_mul_ps(_rows[3], scalar_vec);
    return result;
}

Vector3 Matrix4::transform_point(const Vector3& point) const {
    __m128 p = _mm_set_ps(1.0f, point.z(), point.y(), point.x());
    
    // Multiply each row with the point using AVX
    __m128 x = _mm_shuffle_ps(p, p, _MM_SHUFFLE(0, 0, 0, 0));
    __m128 y = _mm_shuffle_ps(p, p, _MM_SHUFFLE(1, 1, 1, 1));
    __m128 z = _mm_shuffle_ps(p, p, _MM_SHUFFLE(2, 2, 2, 2));
    __m128 w = _mm_shuffle_ps(p, p, _MM_SHUFFLE(3, 3, 3, 3));
    
    #ifdef __FMA__
        __m128 result = _mm_mul_ps(x, _rows[0]);
        result = _mm_fmadd_ps(y, _rows[1], result);
        result = _mm_fmadd_ps(z, _rows[2], result);
        result = _mm_fmadd_ps(w, _rows[3], result);
    #else
        __m128 result = _mm_add_ps(
            _mm_add_ps(
                _mm_mul_ps(x, _rows[0]),
                _mm_mul_ps(y, _rows[1])
            ),
            _mm_add_ps(
                _mm_mul_ps(z, _rows[2]),
                _mm_mul_ps(w, _rows[3])
            )
        );
    #endif
    
    return Vector3(result);
}

Vector3 Matrix4::transform_vector(const Vector3& vector) const {
    __m128 v = _mm_set_ps(0.0f, vector.z(), vector.y(), vector.x());
    
    // Same as transform_point but with w = 0
    __m128 x = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 0));
    __m128 y = _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1));
    __m128 z = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2));
    
    #ifdef __FMA__
        __m128 result = _mm_mul_ps(x, _rows[0]);
        result = _mm_fmadd_ps(y, _rows[1], result);
        result = _mm_fmadd_ps(z, _rows[2], result);
    #else
        __m128 result = _mm_add_ps(
            _mm_add_ps(
                _mm_mul_ps(x, _rows[0]),
                _mm_mul_ps(y, _rows[1])
            ),
            _mm_mul_ps(z, _rows[2])
        );
    #endif
    
    return Vector3(result);
}

Matrix4 Matrix4::transpose() const {
    // Use the well-tested intrinsic macro to avoid subtle shuffle mistakes
    __m128 row0 = _rows[0];
    __m128 row1 = _rows[1];
    __m128 row2 = _rows[2];
    __m128 row3 = _rows[3];

    // The macro transposes the 4x4 matrix stored in row0-row3 in place
    _MM_TRANSPOSE4_PS(row0, row1, row2, row3);

    Matrix4 result;
    result._rows[0] = row0;
    result._rows[1] = row1;
    result._rows[2] = row2;
    result._rows[3] = row3;
    return result;
}

// Matrix inverse using Gauss-Jordan elimination
Matrix4 Matrix4::inverse() const {
    // Create augmented matrix [A | I]
    alignas(16) float augmented[4][8];
    
    // Copy matrix to left half and identity to right half
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            augmented[i][j] = (*this)(i, j);
            augmented[i][j + 4] = (i == j) ? 1.0f : 0.0f;
        }
    }
    
    // Gauss-Jordan elimination
    for (int i = 0; i < 4; ++i) {
        // Find pivot
        int pivot_row = i;
        float max_val = std::abs(augmented[i][i]);
        for (int k = i + 1; k < 4; ++k) {
            float val = std::abs(augmented[k][i]);
            if (val > max_val) {
                max_val = val;
                pivot_row = k;
            }
        }
        
        // Check for singular matrix
        if (max_val < 1e-10f) {
            return identity();
        }
        
        // Swap rows if needed
        if (pivot_row != i) {
            for (int j = 0; j < 8; ++j) {
                std::swap(augmented[i][j], augmented[pivot_row][j]);
            }
        }
        
        // Scale pivot row
        float pivot = augmented[i][i];
        __m128 pivot_vec = _mm_set1_ps(1.0f / pivot);
        __m128 left_half = _mm_loadu_ps(&augmented[i][0]);
        __m128 right_half = _mm_loadu_ps(&augmented[i][4]);
        _mm_storeu_ps(&augmented[i][0], _mm_mul_ps(left_half, pivot_vec));
        _mm_storeu_ps(&augmented[i][4], _mm_mul_ps(right_half, pivot_vec));
        
        // Eliminate column
        for (int k = 0; k < 4; ++k) {
            if (k != i) {
                float factor = augmented[k][i];
                __m128 factor_vec = _mm_set1_ps(factor);
                
                __m128 row_left = _mm_loadu_ps(&augmented[k][0]);
                __m128 pivot_left = _mm_loadu_ps(&augmented[i][0]);
                #ifdef __FMA__
                    row_left = _mm_fnmadd_ps(factor_vec, pivot_left, row_left);
                #else
                    row_left = _mm_sub_ps(row_left, _mm_mul_ps(factor_vec, pivot_left));
                #endif
                _mm_storeu_ps(&augmented[k][0], row_left);
                
                __m128 row_right = _mm_loadu_ps(&augmented[k][4]);
                __m128 pivot_right = _mm_loadu_ps(&augmented[i][4]);
                #ifdef __FMA__
                    row_right = _mm_fnmadd_ps(factor_vec, pivot_right, row_right);
                #else
                    row_right = _mm_sub_ps(row_right, _mm_mul_ps(factor_vec, pivot_right));
                #endif
                _mm_storeu_ps(&augmented[k][4], row_right);
            }
        }
    }
    
    // Extract inverse matrix from right half
    Matrix4 result;
    for (int i = 0; i < 4; ++i) {
        result._rows[i] = _mm_loadu_ps(&augmented[i][4]);
    }
    
    return result;
}

float Matrix4::determinant() const {
    // Calculate determinant using cofactor expansion along first row
    float det = 0.0f;
    
    for (int col = 0; col < 4; ++col) {
        // Calculate 3x3 minor
        float minor[9];
        int minor_idx = 0;
        
        for (int i = 1; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (j != col) {
                    minor[minor_idx++] = (*this)(i, j);
                }
            }
        }
        
        // Calculate 3x3 determinant
        float det3x3 = minor[0] * (minor[4] * minor[8] - minor[5] * minor[7]) -
                       minor[1] * (minor[3] * minor[8] - minor[5] * minor[6]) +
                       minor[2] * (minor[3] * minor[7] - minor[4] * minor[6]);
        
        // Add to total determinant with appropriate sign
        float cofactor = ((col % 2) == 0 ? 1.0f : -1.0f) * det3x3;
        det += (*this)(0, col) * cofactor;
    }
    
    return det;
}

bool Matrix4::is_invertible(float epsilon) const {
    return std::abs(determinant()) > epsilon;
}

bool Matrix4::decompose(Vector3& translation, Vector3& rotation, Vector3& scale) const {
    // Extract translation (last column)
    translation = Vector3((*this)(0, 3), (*this)(1, 3), (*this)(2, 3));
    
    // Extract scale
    Vector3 col0((*this)(0, 0), (*this)(1, 0), (*this)(2, 0));
    Vector3 col1((*this)(0, 1), (*this)(1, 1), (*this)(2, 1));
    Vector3 col2((*this)(0, 2), (*this)(1, 2), (*this)(2, 2));
    
    scale = Vector3(col0.length(), col1.length(), col2.length());
    
    // Check for negative scale (flip)
    if (determinant() < 0) {
        scale = scale * -1.0f;
    }
    
    // Extract rotation by removing scale
    Matrix4 rotation_matrix = *this;
    if (scale.x() != 0) {
        rotation_matrix(0, 0) /= scale.x();
        rotation_matrix(1, 0) /= scale.x();
        rotation_matrix(2, 0) /= scale.x();
    }
    if (scale.y() != 0) {
        rotation_matrix(0, 1) /= scale.y();
        rotation_matrix(1, 1) /= scale.y();
        rotation_matrix(2, 1) /= scale.y();
    }
    if (scale.z() != 0) {
        rotation_matrix(0, 2) /= scale.z();
        rotation_matrix(1, 2) /= scale.z();
        rotation_matrix(2, 2) /= scale.z();
    }
    
    // Convert rotation matrix to Euler angles (simplified)
    rotation.set_x(std::atan2(rotation_matrix(2, 1), rotation_matrix(2, 2)));
    rotation.set_y(std::atan2(-rotation_matrix(2, 0), 
                   std::sqrt(rotation_matrix(2, 1) * rotation_matrix(2, 1) + 
                            rotation_matrix(2, 2) * rotation_matrix(2, 2))));
    rotation.set_z(std::atan2(rotation_matrix(1, 0), rotation_matrix(0, 0)));
    
    return true;
}

void Matrix4::print() const {
    std::cout << std::fixed << std::setprecision(3);
    for (int i = 0; i < 4; ++i) {
        std::cout << "| ";
        for (int j = 0; j < 4; ++j) {
            std::cout << std::setw(8) << (*this)(i, j) << " ";
        }
        std::cout << "|" << std::endl;
    }
    std::cout << std::endl;
} 

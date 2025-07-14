#include "../../include/graphics/mesh.h"
#include <cmath>

Mesh::Mesh() {}

Mesh::~Mesh() {}

void Mesh::add_vertex(const Vertex& vertex) {
    _vertices.push_back(vertex);
}

void Mesh::add_triangle(int v1, int v2, int v3) {
    _indices.push_back(v1);
    _indices.push_back(v2);
    _indices.push_back(v3);
}

Mesh Mesh::create_cube(float size) {
    Mesh mesh;
    float half_size = size * 0.5f;
    
    Vector3 vertices[8] = {
        Vector3(-half_size, -half_size, -half_size), 
        Vector3( half_size, -half_size, -half_size),
        Vector3( half_size,  half_size, -half_size),
        Vector3(-half_size,  half_size, -half_size),
        Vector3(-half_size, -half_size,  half_size),
        Vector3( half_size, -half_size,  half_size),
        Vector3( half_size,  half_size,  half_size),
        Vector3(-half_size,  half_size,  half_size)
    };
    
    Vector3 normals[6] = {
        Vector3( 0,  0, -1), // Front
        Vector3( 0,  0,  1), // Back
        Vector3(-1,  0,  0), // Left
        Vector3( 1,  0,  0), // Right
        Vector3( 0, -1,  0), // Bottom
        Vector3( 0,  1,  0)  // Top
    };
    
    Vector3 colors[6] = {
        Vector3(1, 0, 0), // Red
        Vector3(0, 1, 0), // Green
        Vector3(0, 0, 1), // Blue
        Vector3(1, 1, 0), // Yellow
        Vector3(1, 0, 1), // Magenta
        Vector3(0, 1, 1)  // Cyan
    };
    
    int face_indices[6][4] = {
        {0, 1, 2, 3}, // Front
        {5, 4, 7, 6}, // Back  
        {4, 0, 3, 7}, // Left
        {1, 5, 6, 2}, // Right
        {4, 5, 1, 0}, // Bottom
        {3, 2, 6, 7}  // Top
    };
    
    int triangle_winding[6][6] = {
        {0, 1, 2, 0, 2, 3},
        {0, 2, 1, 0, 3, 2},
        {0, 1, 2, 0, 2, 3},
        {0, 2, 1, 0, 3, 2},
        {0, 2, 1, 0, 3, 2},
        {0, 1, 2, 0, 2, 3}
    };
    
    for (int face = 0; face < 6; ++face) {
        int base_vertex = mesh._vertices.size();
        
        for (int i = 0; i < 4; ++i) {
            mesh.add_vertex(Vertex(
                vertices[face_indices[face][i]],
                normals[face],
                colors[face]
            ));
        }
        
        mesh.add_triangle(base_vertex + triangle_winding[face][0], 
                         base_vertex + triangle_winding[face][1], 
                         base_vertex + triangle_winding[face][2]);
        mesh.add_triangle(base_vertex + triangle_winding[face][3], 
                         base_vertex + triangle_winding[face][4], 
                         base_vertex + triangle_winding[face][5]);
    }
    
    return mesh;
}

Mesh Mesh::create_sphere(float radius, int segments) {
    Mesh mesh;
    
    mesh.add_vertex(Vertex(
        Vector3(0, radius, 0),
        Vector3(0, 1, 0),
        Vector3(1, 1, 1)
    ));
    
    for (int lat = 1; lat < segments; ++lat) {
        float theta = static_cast<float>(lat) * M_PI / segments;
        float sin_theta = std::sin(theta);
        float cos_theta = std::cos(theta);
        
        for (int lon = 0; lon < segments * 2; ++lon) {
            float phi = static_cast<float>(lon) * 2.0f * M_PI / (segments * 2);
            float sin_phi = std::sin(phi);
            float cos_phi = std::cos(phi);
            
            Vector3 position(
                radius * sin_theta * cos_phi,
                radius * cos_theta,
                radius * sin_theta * sin_phi
            );
            
            Vector3 normal = position.normalized();
            Vector3 color(
                (normal.x() + 1.0f) * 0.5f,
                (normal.y() + 1.0f) * 0.5f,
                (normal.z() + 1.0f) * 0.5f
            );
            
            mesh.add_vertex(Vertex(position, normal, color));
        }
    }
    
    mesh.add_vertex(Vertex(
        Vector3(0, -radius, 0),
        Vector3(0, -1, 0),
        Vector3(1, 1, 1)
    ));
    
    int rings = segments - 1;
    int points_per_ring = segments * 2;
    
    for (int i = 0; i < points_per_ring; ++i) {
        int next = (i + 1) % points_per_ring;
        mesh.add_triangle(0, i + 1, next + 1);
    }
    
    for (int ring = 0; ring < rings - 1; ++ring) {
        int current_ring_start = 1 + ring * points_per_ring;
        int next_ring_start = 1 + (ring + 1) * points_per_ring;
        
        for (int i = 0; i < points_per_ring; ++i) {
            int next = (i + 1) % points_per_ring;
            
            mesh.add_triangle(
                current_ring_start + i,
                next_ring_start + i,
                current_ring_start + next
            );
            
            mesh.add_triangle(
                current_ring_start + next,
                next_ring_start + i,
                next_ring_start + next
            );
        }
    }
    
    int last_ring_start = 1 + (rings - 1) * points_per_ring;
    int bottom_vertex = mesh._vertices.size() - 1;
    
    for (int i = 0; i < points_per_ring; ++i) {
        int next = (i + 1) % points_per_ring;
        mesh.add_triangle(
            last_ring_start + next,
            last_ring_start + i,
            bottom_vertex
        );
    }
    
    return mesh;
}

Mesh Mesh::create_plane(float width, float height) {
    Mesh mesh;
    
    float half_width = width * 0.5f;
    float half_height = height * 0.5f;
    
    mesh.add_vertex(Vertex(
        Vector3(-half_width, 0, -half_height),
        Vector3(0, 1, 0),
        Vector3(1, 1, 1)
    ));
    mesh.add_vertex(Vertex(
        Vector3(half_width, 0, -half_height),
        Vector3(0, 1, 0),
        Vector3(1, 1, 1)
    ));
    mesh.add_vertex(Vertex(
        Vector3(half_width, 0, half_height),
        Vector3(0, 1, 0),
        Vector3(1, 1, 1)
    ));
    mesh.add_vertex(Vertex(
        Vector3(-half_width, 0, half_height),
        Vector3(0, 1, 0),
        Vector3(1, 1, 1)
    ));
    
    mesh.add_triangle(0, 1, 2);
    mesh.add_triangle(0, 2, 3);
    
    return mesh;
}

Mesh Mesh::create_triangle(float size) {
    Mesh mesh;
    
    float half_size = size * 0.5f;
    
    mesh.add_vertex(Vertex(
        Vector3(0, 0, half_size),
        Vector3(0, 1, 0),
        Vector3(1, 0, 0)
    ));
    mesh.add_vertex(Vertex(
        Vector3(-half_size, 0, -half_size),
        Vector3(0, 1, 0),
        Vector3(0, 1, 0)
    ));
    mesh.add_vertex(Vertex(
        Vector3(half_size, 0, -half_size),
        Vector3(0, 1, 0),
        Vector3(0, 0, 1)
    ));
    
    mesh.add_triangle(0, 1, 2);
    
    return mesh;
}

void Mesh::calculate_normals() {
    for (auto& vertex : _vertices) {
        vertex.normal = Vector3(0, 0, 0);
    }
    
    for (size_t i = 0; i < _indices.size(); i += 3) {
        int i0 = _indices[i];
        int i1 = _indices[i + 1];
        int i2 = _indices[i + 2];
        
        Vector3 v1 = _vertices[i1].position - _vertices[i0].position;
        Vector3 v2 = _vertices[i2].position - _vertices[i0].position;
        Vector3 normal = v1.cross(v2).normalized();
        
        _vertices[i0].normal = _vertices[i0].normal + normal;
        _vertices[i1].normal = _vertices[i1].normal + normal;
        _vertices[i2].normal = _vertices[i2].normal + normal;
    }
    
    for (auto& vertex : _vertices) {
        vertex.normal.normalize();
    }
}

void Mesh::clear() {
    _vertices.clear();
    _indices.clear();
} 

#pragma once

#include "../math/vector3.h"
#include <vector>

struct Vertex {
    Vector3 position;
    Vector3 normal;
    Vector3 color;
    
    Vertex(const Vector3& pos = Vector3(), 
           const Vector3& norm = Vector3(0, 1, 0), 
           const Vector3& col = Vector3(1, 1, 1))
        : position(pos), normal(norm), color(col) {}
};

class Mesh {
public:
    Mesh();
    ~Mesh();
    
    void add_vertex(const Vertex& vertex);
    void add_triangle(int v1, int v2, int v3);
    
    const std::vector<Vertex>& vertices() const { return _vertices; }
    const std::vector<int>& indices() const { return _indices; }
    
    static Mesh create_cube(float size = 1.0f);
    static Mesh create_sphere(float radius = 1.0f, int segments = 16);
    static Mesh create_plane(float width = 1.0f, float height = 1.0f);
    static Mesh create_triangle(float size = 1.0f);
    
    void calculate_normals();
    void clear();
    size_t vertex_count() const { return _vertices.size(); }
    size_t triangle_count() const { return _indices.size() / 3; }
    
private:
    std::vector<Vertex> _vertices;
    std::vector<int> _indices;
}; 

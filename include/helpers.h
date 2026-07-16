//
// Created by hhabib on 05/07/2026.
//
#pragma once
#include <cmath>
#include <cstdint>
#include <vector>
#include <set>
#include <algorithm>

struct Vector3 {
    float x = 0, y = 0, z = 0;

    Vector3 operator-(const Vector3& o) const { return {x - o.x, y - o.y, z - o.z}; }
    Vector3 operator+(const Vector3& o) const { return {x + o.x, y + o.y, z + o.z}; }
    Vector3 operator*(float s) const { return {x * s, y * s, z * s}; }

    float magnitude() const { return std::sqrt(x*x + y*y + z*z); }
    Vector3 normalized() const { float m = magnitude(); return {x/m, y/m, z/m}; }
};

inline float dot(Vector3 a, Vector3 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

inline Vector3 cross(Vector3 a, Vector3 b) {
    return { a.y*b.z - a.z*b.y,
             a.z*b.x - a.x*b.z,
             a.x*b.y - a.y*b.x };
}

inline float distance(Vector3 a, Vector3 b) {
    return (a - b).magnitude();
}


struct Triangle {
    int a, b, c;

    Vector3 normal(const std::vector<Vector3>& verts) const {
        Vector3 u = verts[b] - verts[a];
        Vector3 v = verts[c] - verts[a];
        return cross(u, v);
    }

    Vector3 center(const std::vector<Vector3>& verts) const {
        const Vector3& aVert = verts[a];
        const Vector3& bVert = verts[b];
        const Vector3& cVert = verts[c];
        return { (aVert.x + bVert.x + cVert.x) / 3.0f,
                 (aVert.y + bVert.y + cVert.y) / 3.0f,
                 (aVert.z + bVert.z + cVert.z) / 3.0f };
    }
};

struct Edge {
    uint32_t a, b;
};

struct Mesh {
    std::vector<Vector3> vertices;
    std::vector<Triangle> triangles;
    std::set<Edge> edges;

};
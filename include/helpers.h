//
// Created by hhabib on 05/07/2026.
//
#pragma once
#include <vector>
#include <set>
#include <algorithm>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>

inline float dot(glm::vec3 a, glm::vec3 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

inline glm::vec3 cross(glm::vec3 a, glm::vec3 b) {
    return { a.y*b.z - a.z*b.y,
             a.z*b.x - a.x*b.z,
             a.x*b.y - a.y*b.x };
}

inline float distance(glm::vec3 a, glm::vec3 b) {
    return glm::length(a - b);
}


struct Triangle {
    uint32_t a, b, c;

    [[nodiscard]] glm::vec3 normal(const std::vector<glm::vec3>& verts) const {
        glm::vec3 u = verts[b] - verts[a];
        glm::vec3 v = verts[c] - verts[a];
        return cross(u, v);
    }

    [[nodiscard]] glm::vec3 center(const std::vector<glm::vec3>& verts) const {
        const glm::vec3& aVert = verts[a];
        const glm::vec3& bVert = verts[b];
        const glm::vec3& cVert = verts[c];
        return { (aVert.x + bVert.x + cVert.x) / 3.0f,
                 (aVert.y + bVert.y + cVert.y) / 3.0f,
                 (aVert.z + bVert.z + cVert.z) / 3.0f };
    }
};

struct Edge {
    uint32_t a, b;

    Edge(uint32_t _a, uint32_t _b) {
      a = std::min(_a, _b);
      b = std::max(_a, _b);
    }

  bool operator<(const Edge& other) const {
      if (a != other.a) return a < other.a;
      return b < other.b;
    }
};

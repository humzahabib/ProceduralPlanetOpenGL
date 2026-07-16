//
// Created by hhabib on 16/07/2026.
//
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/rotate_vector.hpp"
#include "../include/Mesh.h"
#include <map>

Mesh::Mesh() {
  vertices = {};
  triangles = {};
}

Mesh::Mesh(std::vector<glm::vec3> vertices, std::vector<Triangle> triangles) {
  this -> vertices = vertices;
  this -> triangles = triangles;
}

void Mesh::Subdivide() {
  std::vector<glm::vec3> newVertices;
  std::vector<Triangle> newTriangles = {};

  std::map<Edge, uint32_t> edges;

  newVertices = vertices;

  uint32_t v4, v5, v6;

  for (const Triangle& tri: triangles) {
    auto it = edges.find(Edge(tri.a, tri.b));
    if (it == edges.end()) {
      glm::vec3 vertex4 = glm::slerp(vertices[tri.a], vertices[tri.b], 0.5f);
      v4 = newVertices.size();
      newVertices.push_back(vertex4);
      edges.insert({Edge(tri.a, tri.b), v4});
    }
    else {
      v4 = it -> second;
    }

    it = edges.find(Edge(tri.b, tri.c));
    if (it == edges.end()) {
      glm::vec3 vertex5 = glm::slerp(vertices[tri.b], vertices[tri.c], 0.5f);
      v5 = newVertices.size();
      newVertices.push_back(vertex5);
      edges.insert({Edge(tri.b, tri.c), v5});
    }
    else
      v5 = it -> second;


    it = edges.find(Edge(tri.c, tri.a));
    if (it == edges.end()) {
      glm::vec3 vertex6 = glm::slerp(vertices[tri.c], vertices[tri.a], 0.5f);
      v6 = newVertices.size();
      newVertices.push_back(vertex6);
      edges.insert({Edge(tri.c, tri.a), v6});
    }
    else
      v6 = it -> second;

    newTriangles.push_back(Triangle(tri.a, v4, v6));
    newTriangles.push_back(Triangle(v4, tri.b, v5));
    newTriangles.push_back(Triangle(v6, v5, tri.c));
    newTriangles.push_back(Triangle(v6, v4, v5));
  }

  vertices = newVertices;
  triangles = newTriangles;

}

void Mesh::loopSubdivide(int iterations) {
  for (int i = 0; i < iterations; i++)
    Subdivide();
}
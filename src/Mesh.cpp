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

Mesh::Mesh(glm::vec3 position, std::vector<glm::vec3> vertices, std::vector<Triangle> triangles) {

  for (const auto& vertex : vertices)
    this -> vertices.push_back(vertex + position);
}

void Mesh::Subdivide() {
  std::vector<glm::vec3> newVertices;
  std::vector<Triangle> newTriangles = {};

  std::map<Edge, uint32_t> edges;

  newVertices = vertices;

  uint32_t v4, v5, v6;

  float radius = glm::length(vertices[0]);

  for (const Triangle& tri : triangles) {
    auto it = edges.find(Edge(tri.a, tri.b));
    if (it == edges.end()) {
      glm::vec3 vertex4 = glm::slerp(glm::normalize(vertices[tri.a]), glm::normalize(vertices[tri.b]), 0.5f) * radius;
      v4 = newVertices.size();
      newVertices.push_back(vertex4);
      edges.insert({Edge(tri.a, tri.b), v4});
    }
    else
      v4 = it -> second;

    it = edges.find(Edge(tri.b, tri.c));
    if (it == edges.end()) {
      glm::vec3 vertex5 = glm::slerp(glm::normalize(vertices[tri.b]), glm::normalize(vertices[tri.c]), 0.5f) * radius;
      v5 = newVertices.size();
      newVertices.push_back(vertex5);
      edges.insert({Edge(tri.b, tri.c), v5});
    }
    else
      v5 = it -> second;


    it = edges.find(Edge(tri.c, tri.a));
    if (it == edges.end()) {
      glm::vec3 vertex6 = glm::slerp(glm::normalize(vertices[tri.c]), glm::normalize(vertices[tri.a]), 0.5f) * radius;
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
  for (int i = 0; i < iterations; i++) {
    Subdivide();
    calculateNormals();
  }

}

void Mesh::calculateNormals() {
  normals.assign(vertices.size(), glm::vec3(0.0f));

  for (const auto &tri : triangles) {
    glm::vec3 normal = tri.normal(vertices);

    normals[tri.a] += normal;
    normals[tri.b] += normal;
    normals[tri.c] += normal;
  }

  for (int i = 0; i < normals.size(); i++) {
    glm::vec3 newNormal = glm::normalize(normals[i]);
    normals[i] = newNormal;
  }

}

void Mesh::setPosition(glm::vec3 _position) {
  for (auto& vertex : vertices) {
    vertex = vertex + _position;
    position = _position;
  }
}

int Mesh::writeToFile() {
  std::ofstream verticesFile("../configs/vertices.txt");
  if (!verticesFile) {
    std::cout << "ERROR: Couldn't open the vertices file for writing" << std::endl;
    return 1;
  }
  for (const glm::vec3& v : vertices)
    verticesFile << v.x << ", " << v.y << ", " << v.z << std::endl;
  verticesFile.close();

  std::ofstream trianglesFile("../configs/triangles.txt");
  for (const Triangle& t : triangles)
    trianglesFile << t.a << ", " << t.b << ", " << t.c << std::endl;
  trianglesFile.close();

  return 0;
}

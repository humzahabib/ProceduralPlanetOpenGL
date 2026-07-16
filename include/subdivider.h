//
// Created by hhabib on 16/07/2026.
//

#ifndef PROCEDURALPLANETOPENGL_SUBDIVIDER_H
#define PROCEDURALPLANETOPENGL_SUBDIVIDER_H

#include "./helpers.h"
#include "glm/glm.hpp"
#include <map>









void Subdivide(Mesh mesh, int iterations, Mesh *out) {
  *out = Mesh();
  Mesh newMesh;

  for (int i = 0; i < iterations; i++) {
    newMesh = Mesh();
    int verticesAdded = 0;
    int v1Idx = -1, v2Idx = -1, v3Idx = -1, v4Idx = -1, v5Idx = -1, v6Idx = -1;
    std::map<Edge, int> edges;

    for (const Triangle& tri: mesh.triangles) {
      glm::vec3 v1 = mesh.vertices[tri.a];
      v1Idx = verticesAdded;
      verticesAdded++;

      glm::vec3 v2 = mesh.vertices[tri.b];
      v2Idx = verticesAdded;
      verticesAdded++;

      glm::vec3 v3 = mesh.vertices[tri.c];
      v3Idx = verticesAdded;
      verticesAdded++;

      auto it = edges.find(Edge(tri.a, tri.b));
      glm::vec3 v4;

      if (it == edges.end()) {
        v4 = glm::slerp(v1, v2, 0.5f);
        newMesh.vertices.push_back(v4);
        v4Idx = verticesAdded;
        verticesAdded++;
        edges.insert({Edge(tri.a, tri.b), v4Idx});
      }
      else
        v4Idx = it -> second;

      it = edges.find(Edge(tri.b, tri.c));
      glm::vec3 v5;

      if ()

      it = edges.find(Edge(tri.b, tri.c));
      glm::vec3 v5 = (it == edges.end()) ? glm::slerp(v2, v3, 0.5f) : newMesh.vertices[it -> second];

      it = edges.find(Edge(tri.c, tri.a));
      glm::vec3 v6 = (it == edges.end()) ? glm::slerp(v3, v1, 0.5f) : newMesh.vertices[it -> second];

      newMesh.vertices.push_back(v1);
      newMesh.vertices.push_back(v2);
      newMesh.vertices.push_back(v3);
      newMesh.vertices.push_back(v4);
      newMesh.vertices.push_back(v5);
      newMesh.vertices.push_back(v6);



      auto tri1 = Triangle(verticesAdded, verticesAdded + 3, verticesAdded + 5);
      newMesh.triangles.push_back(tri1);

      auto tri2 = Triangle(verticesAdded + 3, verticesAdded + 1, verticesAdded + 4);
      newMesh.triangles.push_back(tri2);

      auto tri3 = Triangle(verticesAdded + 5, verticesAdded + 3, verticesAdded + 4);
      newMesh.triangles.push_back(tri3);

      auto tri4 = Triangle(verticesAdded + 5, verticesAdded + 4, verticesAdded + 2);
      newMesh.triangles.push_back(tri4);
      verticesAdded += 6;
    }
    mesh = newMesh;
  }
  *out = newMesh;

}


#endif  // PROCEDURALPLANETOPENGL_SUBDIVIDER_H

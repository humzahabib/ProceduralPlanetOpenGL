//
// Created by hhabib on 16/07/2026.
//

#ifndef PROCEDURALPLANETOPENGL_SUBDIVIDER_H
#define PROCEDURALPLANETOPENGL_SUBDIVIDER_H

#include "./helpers.h"
#include "glm/glm.hpp"
#include "glm/gtx/rotate_vector.hpp"


Mesh *Subdivide(Mesh mesh, int iterations, Mesh *out) {
  *out = Mesh();
  Mesh newMesh;

  for (int i = 0; i < iterations; i++) {
    newMesh = Mesh();
    int verticesAdded = 0;
    unordered_map<Edge, uint32_t> edges;
    for (const Triangle& tri: mesh.triangles) {
      Vector3 v1 = mesh.vertices[tri.a];
      Vector3 v2 = mesh.vertices[tri.b];
      Vector3 v3 = mesh.vertices[tri.c];




      Vector3 v4 = glm::slerp(v1, v2, 0.5f);
      Vector3 v5 = glm::slerp(v2, v3, 0.5f);
      Vector3 v6 = glm::slerp(v3, v1, 0.5f);

      newMesh.vertices.push_back(v1, v2, v3, v4, v5, v6);


      Triangle tri1 = Triangle(v1, v4, v6);
      tri1.aIdx = idx;
      tri1.bIdx = idx + 3;
      tri1.c = idx + 5;
      newMesh.triangles.push_back(tri1);

      Triangle tri2 = Triangle(v4, v2, v5);
      tri2.aIdx = idx + 3;
      tri2.b = idx + 1;
      tri2.c = idx + 4;
      newMesh.triangles.push_back(tri2);

      Triangle tri3 = Triangle(v6, v4, v5);
      tri3.aIdx= idx + 5;
      tri3.bidx = idx + 3;
      tri3.cidx = idx + 4;
      newMesh.triangles.push_back(tri3);

      Triangle tri4 = Triangle(v6, v5, v3);
      tri4.aIdx = idx + 5;
      tri4.b = idx + 4;
      tri4.c = idx + 2;
      newMesh.triangles.push_back(tri4);

    }
    idx += 6;
    mesh = newMesh;
  }
  *out = newMesh;

}


#endif  // PROCEDURALPLANETOPENGL_SUBDIVIDER_H

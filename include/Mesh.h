//
// Created by hhabib on 16/07/2026.
//

#ifndef PROCEDURALPLANETOPENGL_MESH_H
#define PROCEDURALPLANETOPENGL_MESH_H

#include <helpers.h>

class Mesh {
public:
  std::vector<glm::vec3> vertices;
  std::vector<Triangle> triangles;

  Mesh();
  Mesh(std::vector<glm::vec3> vertices, std::vector<Triangle> triangles);

  void loopSubdivide(int iterations);


private:
  void Subdivide();

};

#endif  // PROCEDURALPLANETOPENGL_MESH_H

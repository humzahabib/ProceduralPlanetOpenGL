//
// Created by hhabib on 16/07/2026.
//

#ifndef PROCEDURALPLANETOPENGL_MESH_H
#define PROCEDURALPLANETOPENGL_MESH_H

#include <terrain/helpers.h>

#include <fstream>
#include <iostream>

class Mesh {
public:
  unsigned int vao, vbo, ebo, indexCount;
  glm::vec3 position;
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec3> normals;
  std::vector<Triangle> triangles;

  Mesh();
  Mesh(glm::vec3 position, std::vector<glm::vec3> vertices, std::vector<Triangle> triangles);

  bool initialize();

  void loopSubdivide(int iterations);
  void calculateNormals();
  void setPosition(glm::vec3 _position);
  void draw() const;

  int writeToFile();


private:
  void Subdivide();

};


#endif  // PROCEDURALPLANETOPENGL_MESH_H

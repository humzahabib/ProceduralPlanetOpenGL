//
// Created by hhabib on 06/07/2026.
//

#ifndef PROCEDURALPLANETOPENGL_PLANETGENERATOR_H
#define PROCEDURALPLANETOPENGL_PLANETGENERATOR_H

#include <./helpers.h>

#include <set>
#include <vector>

#include "Mesh.h"

void getIcosahedronVertices(float size, glm::vec3 *vertices);
std::vector<std::vector<int>> getNeighborVertices(glm::vec3 *vertices);
std::set<std::tuple<int, int, int>>  getTriangles(std::vector<std::vector<int>> neighbors);
void getAlignedTriangles(glm::vec3 *vertices, std::set<std::tuple<int, int, int>> &triangles);
void genIcosahedron(float size, Mesh& mesh);
int writeToFile();

#endif //PROCEDURALPLANETOPENGL_PLANETGENERATOR_H

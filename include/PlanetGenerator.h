//
// Created by hhabib on 06/07/2026.
//

#ifndef PROCEDURALPLANETOPENGL_PLANETGENERATOR_H
#define PROCEDURALPLANETOPENGL_PLANETGENERATOR_H

#include <cmath>
#include "./shader.h"
#include <vector>


void getIcosahedronVertices(float size, float *verticesArray);
std::vector<std::vector<int>> getNeighborVertices(float *vertices);

std::vector<std::vector<int>> getTriangles(std::vector<std::vector<int>> neighbors);


#endif //PROCEDURALPLANETOPENGL_PLANETGENERATOR_H

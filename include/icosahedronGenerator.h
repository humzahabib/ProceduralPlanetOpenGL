//
// Created by hhabib on 06/07/2026.
//

#ifndef PROCEDURALPLANETOPENGL_PLANETGENERATOR_H
#define PROCEDURALPLANETOPENGL_PLANETGENERATOR_H

#include <./helpers.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <iostream>
#include <set>
#include <vector>

float dot(Vector3 a, Vector3 b);
Vector3 cross(Vector3 a, Vector3 b);


void getIcosahedronVertices(float size, Vector3 *vertices);
std::vector<std::vector<int>> getNeighborVertices(Vector3 *vertices);
std::set<std::tuple<int, int, int>>  getTriangles(std::vector<std::vector<int>> neighbors);
void getAlignedTriangles(Vector3 *vertices, std::set<std::tuple<int, int, int>> &triangles);
void genIcosahedron(float size, int *indicesArray, float *vertices);
int writeToFile();

float distance(Vector3 a, Vector3 b);

#endif //PROCEDURALPLANETOPENGL_PLANETGENERATOR_H

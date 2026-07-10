//
// Created by hhabib on 06/07/2026.
//

#ifndef PROCEDURALPLANETOPENGL_PLANETGENERATOR_H
#define PROCEDURALPLANETOPENGL_PLANETGENERATOR_H

#include <cmath>
#include <vector>
#include <set>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <array>



struct Vector3
{
  float x, y, z;

  float magnitude()
  {return sqrt(x*x + y*y + z*z);}
};
Vector3 cross(Vector3 a, Vector3 b);


struct Triangle {
  Vector3 a, b, c;

  Vector3 normal()
  {
    Vector3 u = Vector3(b.x - a.x, b.y - a.y, b.z - a.z);
    Vector3 v = Vector3(c.x - a.x, c.y - a.y, c.z - a.z);
    return cross(u, v);
  }

  Vector3 center()
  {
    return Vector3(
            a.x + b.x + c.x / 3,
            a.y + b.y + c.y / 3,
            a.z + b.z + c.z / 3
            );
  }
};





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

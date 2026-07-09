//
// Created by hhabib on 05/07/2026.
//

#include "../include/PlanetConfigs.h"
#include "../include/PlanetGenerator.h"



// sideB longer

void getIcosahedronVertices(float size, float *vertices) {
    float gr = (1 + std::sqrt(5)) / 2;
    float hypo = size;
    float a = std::sqrt((size * size) / (1 + (gr * gr)));
    float sideA = size * a;
    float sideB = sideA * gr;

    float myVertex[] = {
        // XY Plane
         sideA,  sideB,  0,
        -sideA,  sideB,  0,
         sideA, -sideB,  0,
        -sideA, -sideB,  0, //3

        // XZ Plane
         sideB, 0,  sideA,
        -sideB, 0,  sideA,
         sideB, 0, -sideA,
        -sideB, 0, -sideA,//7

       // YZ Plane
         0,  sideA,  sideB,
         0, -sideA,  sideB,
         0,  sideA, -sideB,
         0, -sideA, -sideB,//11

    };

    for (int i = 0; i < 36; i++)
     *(vertices + i) = myVertex[i];

}


std::vector<std::vector<int>> getNeighborVertices(float *vertices) {

 float minEdgeLength = 9999;

 std::vector<std::vector<int>> neighbors(12);


 for (int i = 0; i < 12; i++) {
  int basei = i * 3;
  float x1 = vertices[basei + 0];
  float y1 = vertices[basei + 1];
  float z1 = vertices[basei + 2];
  for (int j = 0; j < 12; j++) {
   int basej = j * 3;
   float x2 = vertices[basej];
   float y2 = vertices[basej + 1];
   float z2 = vertices[basej + 2];

   float distance = std::sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2) + pow(z2 - z1, 2));

   if (distance - minEdgeLength < 0 && i != j)
    minEdgeLength = distance;
  }
 }
 std::cout << "Min edge length = " << minEdgeLength << '\n';

 for (int i = 0; i < 12; i++) {
  int basei = i * 3;
  float x1 = vertices[basei + 0];
  float y1 = vertices[basei + 1];
  float z1 = vertices[basei + 2];
  for (int j = 0; j < 12; j++) {
   int basej = j * 3;
   float x2 = vertices[basej];
   float y2 = vertices[basej + 1];
   float z2 = vertices[basej + 2];

   float distance = std::sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2) + pow(z2 - z1, 2));

   if (fabs(distance - minEdgeLength) < 0.0000001f && i != j)
    neighbors[i].push_back(j);

  }
 }

 return neighbors;

}


std::vector<std::vector<int> > getTriangles(std::vector<std::vector<int> > neighbors) {
 for (int i = 0; i < 12; i++) {
  continue;
 }
}

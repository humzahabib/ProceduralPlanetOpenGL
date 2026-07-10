//
// Created by hhabib on 05/07/2026.
//
#include "../include/icosahedronGenerator.h"

float dot(Vector3 a, Vector3 b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

Vector3 cross(Vector3 a, Vector3 b)
{
	return Vector3(
	a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x);
}


float distance(Vector3 a, Vector3 b) {
  return (sqrt(pow(a.x - b.x, 2) +
    pow(a.y - b.y, 2) +
    pow(a.z - b.z, 2)
    ));
}


// sideB longer

void getIcosahedronVertices(float size, Vector3 *vertices) {
    float gr = (1 + std::sqrt(5)) / 2;
    float hypo = size;
    float a = std::sqrt((size * size) / (1 + (gr * gr)));
    float sideA = size * a;
    float sideB = sideA * gr;

    Vector3 myVertex[] = {
        // XY Plane
        Vector3(sideA,  sideB,  0),
        Vector3(-sideA,  sideB,  0),
        Vector3(sideA, -sideB,  0),
        Vector3(-sideA, -sideB,  0), //3

        // XZ Plane
        Vector3(sideB, 0, sideA),
        Vector3(-sideB, 0, sideA),
        Vector3(sideB, 0, -sideA),
        Vector3(-sideB, 0, -sideA),//7

       // YZ Plane
         Vector3(0,  sideA,  sideB),
         Vector3(0, -sideA,  sideB),
         Vector3(0,  sideA, -sideB),
         Vector3(0, -sideA, -sideB),//11

    };

  for (int i = 0; i < 12; i++) {
    vertices[i] = myVertex[i];
  }


}


std::vector<std::vector<int>> getNeighborVertices(Vector3* vertices) {

 float minEdgeLength = 9999;

 std::vector<std::vector<int>> neighbors(12);


 for (int i = 0; i < 12; i++) {
  for (int j = 0; j < 12; j++) {
   float dist = distance(vertices[i], vertices[j]);

   if (dist - minEdgeLength < 0 && i != j)
    minEdgeLength = dist;
  }
 }
 std::cout << "Min edge length = " << minEdgeLength << '\n';

 for (int i = 0; i < 12; i++) {
  for (int j = 0; j < 12; j++) {

   float dist = distance(vertices[i], vertices[j]);

   if (fabs(dist - minEdgeLength) < 0.0000001f && i != j)
    neighbors[i].push_back(j);

  }
 }

 return neighbors;

}


std::set<std::tuple<int, int, int>> getTriangles(std::vector<std::vector<int>> neighbors) {

  std::set<std::tuple<int, int, int>> triangles;

  for (int i = 0; i < 12; i++) {
    std::vector<int> currentVector = neighbors[i];

    for (int j = 0; j < 5; j++) {
     int currentPivotNeighbor = currentVector[j];
     std::vector<int> neighborsNeighbors = neighbors[currentPivotNeighbor];

     for (int k = j + 1; k < 5; k++) {
      int thirdNeighbor = currentVector[k];
      if (std::find(neighborsNeighbors.begin(), neighborsNeighbors.end(), thirdNeighbor) != neighborsNeighbors.end()) {
       std::array<int, 3> temp = {i, currentPivotNeighbor, thirdNeighbor};
       std::sort(temp.begin(), temp.end());
       triangles.insert({temp[0], temp[1], temp[2]});
      }
   }
  }
 }


 return triangles;
}

void getAlignedTriangles(Vector3 *vertices, std::set<std::tuple<int, int, int>> &triangles) {


  std::set<std::tuple<int, int, int>> fixed;
  for (const std::tuple<int, int, int>& tri: triangles) {
    auto [a, b, c] = tri;

    Triangle triangle = Triangle(vertices[a], vertices[b], vertices[c]);
    float dotProduct = dot(triangle.center(), triangle.normal());




    if (dotProduct < 0)
      fixed.insert({a, c, b});
    else
      fixed.insert(tri);
  }


  triangles = std::move(fixed);

}

void genIcosahedron(float size, int* indicesArray, float* verticesArray) {
  Vector3 vertices[12];
  getIcosahedronVertices(1.0f, vertices);


  std::vector<std::vector<int>> neighbors = getNeighborVertices(vertices);

  for (int i = 0; i < 12; i++) {
    std::cout << i << " -> ";
    for (int j = 0; j < neighbors[i].size(); j++)
      std::cout << neighbors[i][j] << ", ";
    std::cout << std::endl;
  }

  std::set<std::tuple<int, int, int>> triangles = getTriangles(neighbors);
  getAlignedTriangles(vertices, triangles);



  for (int i = 0; i < 12; i++) {
    int base = i * 3;
    verticesArray[base] = vertices[i].x;
    verticesArray[base + 1] = vertices[i].y;
    verticesArray[base + 2] = vertices[i].z;
  }

  int i = 0;
  for (const auto& triangle: triangles) {
    auto [a, b, c] = triangle;
    indicesArray[i] = a;
    indicesArray[i + 1] = b;
    indicesArray[i + 2] = c;
    i += 3;
  }
}

int writeToFile() {
  float vertices[12 * 3];
  int indices[20 * 3];

  genIcosahedron(1, indices, vertices);

  std::ofstream verticesFile("../configs/vertices.txt");

  if (!verticesFile) {
    std::cout << "ERROR: Couldn't open the vertices file for writing" << std::endl;
    return 1;
  }

  for (int i = 0; i < 12; i++) {
    int x = i * 3;
    int y = x + 1;
    int z = x + 2;

    verticesFile << vertices[x] << ", " << vertices[y]  << ", " << vertices[z] << std::endl;

  }

  verticesFile.close();

  std::ofstream trianglesFile("../configs/triangles.txt");
  for (int i = 0; i < 20; i++) {
    int a = i * 3;
    int b = a + 1;
    int c = a + 2;

    trianglesFile << indices[a] << ", " << indices[b] << ", " << indices[c] << std::endl;
  }

  trianglesFile.close();

  return 0;
}

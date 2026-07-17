//
// Created by hhabib on 16/07/2026.
//
#include "../include/icosahedronGenerator.h"

#include <array>
#include <fstream>
#include <iostream>
#include <tuple>

#include "./../include/Mesh.h"

void getIcosahedronVertices(float size, std::vector<glm::vec3>& vertices) {
    float gr = (1.0f + std::sqrt(5.0f)) / 2.0f;
    float a = std::sqrt((size * size) / (1 + gr * gr));
    float sideA = size * a;
    float sideB = sideA * gr;

    vertices = {
        {sideA,  sideB,  0}, {-sideA,  sideB,  0},
        {sideA, -sideB,  0}, {-sideA, -sideB,  0},

        {sideB, 0, sideA}, {-sideB, 0, sideA},
        {sideB, 0, -sideA}, {-sideB, 0, -sideA},

        {0,  sideA,  sideB}, {0, -sideA,  sideB},
        {0,  sideA, -sideB}, {0, -sideA, -sideB},
    };
}

std::vector<std::vector<int>> getNeighborVertices(const std::vector<glm::vec3>& vertices) {
    float minEdgeLength = 9999.0f;
    std::vector<std::vector<int>> neighbors(12);

    for (int i = 0; i < 12; i++)
        for (int j = 0; j < 12; j++)
            if (i != j)
                minEdgeLength = std::min(minEdgeLength, distance(vertices[i], vertices[j]));

    for (int i = 0; i < 12; i++)
        for (int j = 0; j < 12; j++)
            if (i != j && std::fabs(distance(vertices[i], vertices[j]) - minEdgeLength) < 1e-7f)
                neighbors[i].push_back(j);

    return neighbors;
}

std::set<std::tuple<int, int, int>> getTriangleIndices(const std::vector<std::vector<int>>& neighbors) {
    std::set<std::tuple<int, int, int>> triangles;

    for (int i = 0; i < 12; i++) {
        const auto& currentVector = neighbors[i];
        for (int j = 0; j < 5; j++) {
            int pivotNeighbor = currentVector[j];
            const auto& pivotNeighbors = neighbors[pivotNeighbor];
            for (int k = j + 1; k < 5; k++) {
                int thirdNeighbor = currentVector[k];
                if (std::find(pivotNeighbors.begin(), pivotNeighbors.end(), thirdNeighbor) != pivotNeighbors.end()) {
                    std::array<int, 3> temp = {i, pivotNeighbor, thirdNeighbor};
                    std::sort(temp.begin(), temp.end());
                    triangles.insert({temp[0], temp[1], temp[2]});
                }
            }
        }
    }
    return triangles;
}

void genIcosahedron(float size, Mesh& mesh) {
    getIcosahedronVertices(size, mesh.vertices);

    std::vector<std::vector<int>> neighbors = getNeighborVertices(mesh.vertices);
    std::set<std::tuple<int, int, int>> rawTriangles = getTriangleIndices(neighbors);

    mesh.triangles.clear();
    mesh.triangles.reserve(rawTriangles.size());

    for (const auto& [a, b, c] : rawTriangles) {
        Triangle tri(a, b, c);
        if (dot(tri.center(mesh.vertices), tri.normal(mesh.vertices)) < 0)
            tri = Triangle(a, c, b);   // flip winding to face outward
        mesh.triangles.push_back(tri);
    }

}

int writeToFile(const Mesh& mesh) {
    std::ofstream verticesFile("../configs/vertices.txt");
    if (!verticesFile) {
        std::cout << "ERROR: Couldn't open the vertices file for writing" << std::endl;
        return 1;
    }
    for (const glm::vec3& v : mesh.vertices)
        verticesFile << v.x << ", " << v.y << ", " << v.z << std::endl;
    verticesFile.close();

    std::ofstream trianglesFile("../configs/triangles.txt");
    for (const Triangle& t : mesh.triangles)
        trianglesFile << t.a << ", " << t.b << ", " << t.c << std::endl;
    trianglesFile.close();

    return 0;
}

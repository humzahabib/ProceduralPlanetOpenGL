//
// Created by hhabib on 05/07/2026.
//

#include "../include/PlanetConfigs.h"
#include "../include/PlanetGenerator.h"




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
        -sideA, -sideB,  0,//3

        // XZ Plane
         sideA, 0,  sideB,
        -sideA, 0,  sideB,
         sideA, 0, -sideB,
        -sideA, 0, -sideB,//7

       // YZ Plane
         0,  sideA,  sideB,
         0, -sideA,  sideB,
         0,  sideA, -sideB,
         0, -sideA, -sideB,//11

    };

    for (int i = 0; i < 36; i++)
     *(vertices + i) = myVertex[i];

}




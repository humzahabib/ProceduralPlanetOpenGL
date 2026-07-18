//
// Created by hhabib on 18/07/2026.
//

#ifndef PROCEDURALPLANETOPENGL_SPHEREDEFORMER_H
#define PROCEDURALPLANETOPENGL_SPHEREDEFORMER_H

#include <./helpers.h>
#include <./Mesh.h>


#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>


void ApplyPerlinNoiseOnIcosphere(Mesh* icosphere, float height, float frequency, float randomness);

#endif  // PROCEDURALPLANETOPENGL_SPHEREDEFORMER_H

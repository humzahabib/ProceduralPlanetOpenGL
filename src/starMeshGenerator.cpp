//
// Created by hhabib on 22/07/2026.
//

#include "../include/starMeshGenerator.h"

#include <glm/ext/scalar_constants.hpp>
#include <random>

#include "../include/helpers.h"
#include "./../include/Mesh.h"


void generateRandomStars(Mesh &starMesh, float skyboxRadius, int nStars) {
  std::mt19937 rng(std::random_device{}());

  std::uniform_real_distribution<float> phiDist(0.0f, 2.0f * glm::pi<float>());
  std::uniform_real_distribution<float> zDist(-1.0f, 1.0f);

  for (int i = 0; i < nStars; i++) {
    float phi = phiDist(rng);
    float z = zDist(rng);

    float r = sqrt(1.0f - (z * z));


    float x = r * cos(phi);
    float y = r * sin(phi);

    glm::vec3 dir(x, y, z);

    starMesh.vertices.push_back(dir * skyboxRadius);
  }
}
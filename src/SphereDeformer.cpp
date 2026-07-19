//
// Created by hhabib on 18/07/2026.
//

#include "./../include/SphereDeformer.h"

#include <iostream>

void ApplyPerlinNoiseOnIcosphere(Mesh* icosphere, float height, float frequency,
                                  float randomness) {

  std::vector<glm::vec3> newVertices;

  for (auto &vertex : icosphere->vertices) {
    float noise = glm::perlin((vertex + randomness) * frequency) * height;


    glm::vec3 vertexDir = glm::normalize(vertex);

    glm::vec3 heightAddition = vertexDir * noise;

    newVertices.push_back(vertex + heightAddition);
  }

  icosphere -> vertices = newVertices;
}
//
// Created by hhabib on 18/07/2026.
//

#include "./../include/SphereDeformer.h"

void ApplyPerlineNoiseOnIcosphere(Mesh* icosphere, float height, float frequency,
                                  float randomness) {

  std::vector<glm::vec3> newVertices;

  for (auto &vertex : icosphere->vertices) {
    float noise = glm::perlin((vertex + randomness) * frequency) * height;
    float radius = glm::length(vertex);
    glm::vec3 vertexDir = glm::normalize(vertex);

    glm::vec3 newVertex = vertexDir * radius;
    newVertex = newVertex * noise;

    newVertices.push_back(newVertex);
  }

  icosphere -> vertices = newVertices;
}
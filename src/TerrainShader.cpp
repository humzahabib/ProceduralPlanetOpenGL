//
// Created by hhabib on 19/07/2026.
//

#include "./../include/TerrainShader.h"

glm::vec3 iceColor = glm::vec3(0.73, 0.91, 0.92);
glm::vec3 grassColor = glm::vec3(0.49, 0.99, 0.0);
glm::vec3 mudColor = glm::vec3(0.50, 0.45, 0.39);
glm::vec3 oceanColor = glm::vec3(6.0f / 225, 66.0f / 225, 115.0 / 225);

std::vector<glm::vec3> shade(Mesh &mesh) {
  float minHeight = 9999.999f, maxHeight = -1.0f;

  std::vector<float> normalizedHeights = {};
  std::vector<glm::vec3> colors = {};

  for (auto& vertex : mesh.vertices) {
    float h = glm::length(vertex);
    if (h > maxHeight)
      maxHeight = h;
    if (h < minHeight)
      minHeight = h;
  }

  for (const auto& vertex : mesh.vertices) {
    float height = glm::length(vertex);
    float normalized = (height - minHeight) / (maxHeight - minHeight);
    normalizedHeights.push_back(normalized);
  }


  for (const auto& norm : normalizedHeights) {
    if (norm < 0.40f)
      colors.push_back(oceanColor);
    else if (norm < 0.6f)
      colors.push_back(grassColor);
    else if (norm < 0.80f)
      colors.push_back(mudColor);
    else
      colors.push_back(iceColor);

  }

  return colors;
}
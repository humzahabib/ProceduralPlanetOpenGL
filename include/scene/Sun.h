//
// Created by hhabib on 31/07/2026.
//

#include <glm/vec3.hpp>

#include "graphics/Mesh.h"

class Sun {

private:
  float radius;
  float time;

  Mesh mesh;
  glm::vec3 emission;
  Transform transform;
  Shader shader;


public:
  bool initialize();
  void update(float deltaTime);
  void render();
};
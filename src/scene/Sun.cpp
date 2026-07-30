//
// Created by hhabib on 31/07/2026.
//

#include "scene/Sun.h"
#include "terrain/icosahedronGenerator.h"

bool Sun::initialize()
{
  // 1. Generate sphere geometry
  genIcosahedron(radius, mesh);

  // 2. Upload mesh to GPU
  mesh.initialize();

  // 3. Load shaders
  shader.load(
      "shaders/sun/vertexShader.glsl",
      "shaders/sun/fragShader.glsl");

  // 4. Set initial transform
  transform.setPosition(glm::vec3(0.0f));
  transform.setScale(glm::vec3(radius));

  return true;
}

void Sun::update(float deltaTime)
{
  time += deltaTime;
}

void Sun::render() {
  shader.use();

  shader.setFloat("time", time);
  shader.setMat4("model", transform.getModelMatrix());

  mesh.draw();
}
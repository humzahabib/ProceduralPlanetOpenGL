//
// Created by hhabib on 31/07/2026.
//
#include <glad.h>
#include <graphics/FullscreenQuad.h>

#include <glm/detail/type_quat.hpp>

FullscreenQuad::FullscreenQuad()
{
}

FullscreenQuad::~FullscreenQuad() {
  if (n_vbo != 0)
    glDeleteBuffers(1, &n_vbo);
  if (n_vao != 0)
    glDeleteVertexArrays(1, &n_vao);
}

void FullscreenQuad::initialize() {
  glGenBuffers(1, &n_vbo);
  glGenVertexArrays(1, &n_vao);

  glBindVertexArray(n_vao);
  glBindBuffer(GL_ARRAY_BUFFER, n_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, n_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
}

void FullscreenQuad::draw() const {
  glBindVertexArray(n_vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}
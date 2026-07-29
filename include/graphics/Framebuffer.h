//
// Created by hhabib on 27/07/2026.
//

#pragma once

#include <glad.h>

class Framebuffer {
public:
  Framebuffer();
  ~Framebuffer();

  bool createHDR(int width, int height);

  void bind() const;
  void unbind() const;
  void resize(int width, int height);
  unsigned int getColorTexture() const;

private:
  void Destroy();

  unsigned int n_fbo = 0;
  unsigned int n_colorTexture = 0;
  unsigned int n_depthRBO = 0;
  int n_width, n_height;
};
//
// Created by hhabib on 31/07/2026.
//

#pragma once
#include "glad.h"

class Framebuffer {
public:
  Framebuffer();
  ~Framebuffer();

  void Destroy();
  bool createHDR(int width, int height);
  void bind() const;
  void unbind() const;
  GLuint getColorTexture() const;


private:
  GLuint n_depthRBO, n_colorTexture, n_fbo, n_width, n_height;
};
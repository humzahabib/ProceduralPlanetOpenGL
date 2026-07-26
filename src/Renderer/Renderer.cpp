//
// Created by hhabib on 27/07/2026.
//
#include "renderer/Renderer.h"

#include "graphics/Mesh.h"
#include "core/Camera.h"

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

bool Renderer::initialize(int width, int height)
{
  n_width = width;
  n_height = height;

  // Create HDR framebuffer
  n_hdrFrameBuffer.createHDR(width, height);

  // Load tone mapping shader
  n_hdrShader.load(
      "shaders/hdr/vertexShader.glsl",
      "shaders/hdr/fragShader.glsl");

  return true;
}
//
// Created by hhabib on 27/07/2026.
//


#pragma once

#include "graphics/Framebuffer.h"
#include "graphics/FullscreenQuad.h"
#include "graphics/Shader.h"


class Mesh;
class Camera;

class Renderer {
public:
  Renderer();
  ~Renderer();

  bool initialize(int width, int height);
  void BeginFrame();
  void Draw(const Mesh& mesh, Shader &shader);
  void EndFrame();
  void resize(int width, int height);
  void setCamera(Camera* camera);


private:
  Camera *n_camera;
  Framebuffer n_hdrFrameBuffer;
  FullscreenQuad n_fullscreenQuad;
  Shader n_hdrShader;

  int n_width = 0, n_height = 0;
};
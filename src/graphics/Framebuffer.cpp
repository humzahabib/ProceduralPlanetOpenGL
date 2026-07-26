//
// Created by hhabib on 27/07/2026.
//

#include <graphics/Framebuffer.h>

#include <iostream>

Framebuffer::Framebuffer()
{
}

Framebuffer::~Framebuffer()
{
  Destroy();
}

void Framebuffer::Destroy()
{
  if (n_depthRBO != 0)
  {
    glDeleteRenderbuffers(1, &n_depthRBO);
    n_depthRBO = 0;
  }

  if (n_colorTexture != 0)
  {
    glDeleteTextures(1, &n_colorTexture);
    n_colorTexture = 0;
  }

  if (n_fbo != 0)
  {
    glDeleteFramebuffers(1, &n_fbo);
    n_fbo = 0;
  }
}

bool Framebuffer::createHDR(int width, int height)
{
  Destroy();

  n_width = width;
  n_height = height;

  glGenBuffers(1, &n_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, n_fbo);

  glGenTextures(1, &n_colorTexture);
  glBindTexture(GL_TEXTURE_2D, n_colorTexture);
  glTexImage2D(
    GL_TEXTURE_2D, 0, GL_RGBA16F,
    n_width, n_height, 0, GL_RGBA, GL_FLOAT,
    nullptr);

  glTexParameteri(GL_TEXTURE_2D,
                GL_TEXTURE_MIN_FILTER,
                GL_LINEAR);

  glTexParameteri(GL_TEXTURE_2D,
                  GL_TEXTURE_MAG_FILTER,
                  GL_LINEAR);

  glTexParameteri(GL_TEXTURE_2D,
                  GL_TEXTURE_WRAP_S,
                  GL_CLAMP_TO_EDGE);

  glTexParameteri(GL_TEXTURE_2D,
                  GL_TEXTURE_WRAP_T,
                  GL_CLAMP_TO_EDGE);

  glFramebufferTexture2D(
    GL_FRAMEBUFFER,
    GL_COLOR_ATTACHMENT0,
    GL_TEXTURE_2D,
    n_colorTexture,
    0
);

  glGenRenderbuffers(1, &n_depthRBO);

  glBindRenderbuffer(GL_RENDERBUFFER,
                     n_depthRBO);

  glRenderbufferStorage(
      GL_RENDERBUFFER,
      GL_DEPTH_COMPONENT24,
      width,
      height
  );

  glFramebufferRenderbuffer(
      GL_FRAMEBUFFER,
      GL_DEPTH_ATTACHMENT,
      GL_RENDERBUFFER,
      n_depthRBO
  );

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER)
    != GL_FRAMEBUFFER_COMPLETE)
  {
    std::cerr << "Framebuffer is incomplete!\n";
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    Destroy();

    return false;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  return true;

}

void Framebuffer::bind() const
{
  glBindFramebuffer(GL_FRAMEBUFFER, n_fbo);
}

void Framebuffer::unbind() const
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint Framebuffer::getColorTexture() const
{
  return n_colorTexture;
}



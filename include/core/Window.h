//
// Created by hhabib on 27/07/2026.
//

#pragma once

#include <string>

struct GLFWwindow;

class Window {
public:
  Window(int height, int width, const std::string &name);

  ~Window();

  bool Initialize();
  void PollEvents();
  void SwapBuffers();
  bool ShouldClose() const;
  GLFWwindow *getNativeWindow() const;
  int getWidth();
  int getHeight();

private:
  static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
  GLFWwindow* n_window;
  int n_height;
  int n_width;
  std::string n_title;
};
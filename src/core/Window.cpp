//
// Created by hhabib on 27/07/2026.
//

#include "core/Window.h"
#include "./glad.h"
#include <GLFW/glfw3.h>
#include <iostream>

Window::Window(int height, int width, const std::string& name) {
  n_height = height;
  n_width = width;
  n_title = name;
}

bool Window::Initialize() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  n_window = glfwCreateWindow(n_width, n_height, n_title.c_str(), NULL, NULL);
  glfwMakeContextCurrent(n_window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
  }

  glViewport(0, 0, n_width, n_height);
}

void Window::PollEvents() {
  glfwPollEvents();
}

void Window::SwapBuffers() {
  glfwSwapBuffers(n_window);
}

bool Window::ShouldClose() const {
  return glfwWindowShouldClose(n_window);
}

GLFWwindow* Window::getNativeWindow() const {
  return n_window;
}

int Window::getHeight() { return n_height; }

int Window::getWidth() { return n_width; }

void Window::framebufferSizeCallback(GLFWwindow* window,
                                     int width,
                                     int height)
{
  glViewport(0, 0, width, height);
}
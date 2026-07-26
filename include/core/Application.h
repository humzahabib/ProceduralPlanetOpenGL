//
// Created by hhabib on 27/07/2026.
//

#pragma once

#include <memory>

class Window;
class Scene;
class Renderer;

class Application {

public:
  Application();
  ~Application();
  void run();


private:
  void initialize();
  void processInput();
  void update(float deltaTime);
  void render();
  void shutdown();

  std::unique_ptr<Window> window;
  std::unique_ptr<Renderer> renderer;
  std::unique_ptr<Scene> scene;

  bool running = true;


};
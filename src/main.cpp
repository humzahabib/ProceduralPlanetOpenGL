//
// Created by hhabib on 07/07/2026.
//

#include <../include/shader.h>
#include <GLFW/glfw3.h>
#include <glad.h>

#include <chrono>
#include <iostream>

#include "../include/camera.h"
#include "../include/icosahedronGenerator.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "./../include/SphereDeformer.h"
#include "./../include/TerrainShader.h"
#include <glm/gtc/noise.hpp>
#include <./../include/starMeshGenerator.h>


glm::vec3 sunLight = glm::vec3(1.0f, 1.0f, 1.0f);

glm::vec3 sunPos = glm::vec3(-10000.0f, 10000.0f, 10000.0f);
float ambientStrength = 0.0f, specularStrength = 0.1f;
glm::vec3 surfaceColor = glm::vec3(50.0f / 225.0f, 205.0f / 225.0f, 50.0f / 225.0f);



#pragma region Camera Movement Stuff

float lastFrame = 0.0f;
float currentFrame = 0.0f;
float deltaTime;

int lastX = 400, lastY = 300;
bool firstMouse = true;

Camera cam = Camera(glm::vec3(0.0f, 0.0f, -20.0f), glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, 0.0f);;
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

#pragma endregion

float quadVertices[] =
{

  -1,  1, 0, 1,
  -1, -1, 0, 0,
   1, -1, 1, 0,

  -1,  1, 0, 1,
   1, -1, 1, 0,
   1,  1, 1, 1
};

int main() {

  Mesh planet, sun, stars, coronaSun;

  genIcosahedron(8000, planet);
  genIcosahedron(1000, sun);
  genIcosahedron(1200, coronaSun);

  generateRandomStars(stars, 90000.0f, 1000);

  glm::vec3 blueStar = glm::vec3(0.61f, 0.69f, 1.00f);
  glm::vec3 whiteStar = glm::vec3(0.79f, 0.84f, 1.00f);
  glm::vec3 orangeStar = glm::vec3(1.00f, 0.80f, 0.44f);

  std::vector<float> starSizes;
  std::vector<glm::vec3> starColors;


  for (int i = 0; i < 1000; i++) {
    int random = (std::rand() % 100) + 1;

    if (random < 30)
      starSizes.push_back(4.0f);
    else if (random < 50)
      starSizes.push_back(3.0f);
    else if (random < 70)
      starSizes.push_back(2.0f);
    else starSizes.push_back(1.0f);

  }
  for (int i = 0; i < 1000; i++) {
    int random = (std::rand() % 100) + 1;

    if (random < 30)
      starColors.push_back(blueStar);
    else if (random < 50)
      starColors.push_back(orangeStar);
    else
      starColors.push_back(whiteStar);

  }

  std::cout << stars.vertices.size() << '\n';
  std::cout << starSizes.size() << '\n';
  std::cout << starColors.size() << '\n';

  sun.loopSubdivide(8);
  coronaSun.loopSubdivide(6);
  sun.setPosition(sunPos);
  coronaSun.setPosition(sunPos);


  planet.loopSubdivide(7);

  ApplyPerlinNoiseOnIcosphere(&planet, 800.0f, 1.0f/(900 * 2.0f), 0, true);
  ApplyPerlinNoiseOnIcosphere(&planet, 400.0f, 1.0f / (5000.0f * 1.60f), 2, false);
  ApplyPerlinNoiseOnIcosphere(&planet, 0.50f, 1.0 / 25.0f, 3, false);
  std::vector<glm::vec3> colors = shade(planet);
  //
  // std::vector<glm::vec3> sunColor

  std::cout << "Vertices: " << planet.vertices.size() << '\n';
  std::cout << "Triangles: " << planet.triangles.size() << '\n';
  glfwInit();  std::vector<glm::vec3> heightNorms = shade(planet);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(1920, 1080, "Planet", NULL, NULL);

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
      std::cout << "Failed to initialize GLAD" << std::endl;
      return -1;
  }


  glViewport(0, 0, 1920, 1080);


  unsigned int planetVBOs[3], planetVAOs[1], planetEBOs[1];

  glGenBuffers(3, planetVBOs);
  glGenVertexArrays(1, planetVAOs);
  glGenBuffers(1, planetEBOs);

  // planetVAOs InitializeVAO
  glBindVertexArray(planetVAOs[0]);

  // Planet vertex data sending
  glBindBuffer(GL_ARRAY_BUFFER, planetVBOs[0]);
  glBufferData(GL_ARRAY_BUFFER, planet.vertices.size() * sizeof(glm::vec3), planet.vertices.data(), GL_STATIC_DRAW);
  // Planet vertex data read instructions
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // Bind planet triangle buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planetEBOs[0]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, planet.triangles.size() * sizeof(Triangle), planet.triangles.data(), GL_STATIC_DRAW);

  // Planet normal data sending
  glBindBuffer(GL_ARRAY_BUFFER, planetVBOs[1]);
  glBufferData(GL_ARRAY_BUFFER, planet.normals.size() * sizeof(glm::vec3), planet.normals.data(), GL_STATIC_DRAW);
  // Planet normal data read instructions
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);

  // Planet Color data sending
  glBindBuffer(GL_ARRAY_BUFFER, planetVBOs[2]);
  glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);
  // Planet color data read instructions
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(2);

  // Planet shader
  Shader planetShader = Shader("../shaders/planet/vertexShader.glsl", "../shaders/planet/fragShader.glsl");

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_PROGRAM_POINT_SIZE);


  unsigned int sunVBOs[1], sunVAOs[1], sunEBOs[1];


  glGenBuffers(1, sunVBOs);
  glGenVertexArrays(1, sunVAOs);
  glGenBuffers(1, sunEBOs);

  glBindVertexArray(sunVAOs[0]);
  glBindBuffer(GL_ARRAY_BUFFER, sunVBOs[0]);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sunEBOs[0]);

  glBufferData(GL_ARRAY_BUFFER, sun.vertices.size() * sizeof(glm::vec3), sun.vertices.data(), GL_STATIC_DRAW);

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sun.triangles.size() * sizeof(Triangle), sun.triangles.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);


  unsigned int coronaVBO, coronaVAO, coronaEBO;
  glGenBuffers(1, &coronaVBO);
  glGenVertexArrays(1, &coronaVAO);
  glGenBuffers(1, &coronaEBO);

  glBindVertexArray(coronaVAO);
  glBindBuffer(GL_ARRAY_BUFFER, coronaVBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, coronaEBO);

  glBufferData(GL_ARRAY_BUFFER, coronaSun.vertices.size() * sizeof(glm::vec3), coronaSun.vertices.data(), GL_STATIC_DRAW);

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, coronaSun.triangles.size() * sizeof(Triangle), coronaSun.triangles.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);



  unsigned int starsVBOs[3], starsVAOs[1];
  glGenBuffers(3, starsVBOs);
  glGenVertexArrays(1, starsVAOs);

  glBindVertexArray(starsVAOs[0]);
  glBindBuffer(GL_ARRAY_BUFFER, starsVBOs[0]);
  glBufferData(GL_ARRAY_BUFFER, stars.vertices.size() * sizeof(glm::vec3), stars.vertices.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, starsVBOs[1]);
  glBufferData(GL_ARRAY_BUFFER, starSizes.size() * sizeof(float), starSizes.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, starsVBOs[2]);
  glBufferData(GL_ARRAY_BUFFER, starColors.size() * sizeof(glm::vec3), starColors.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(2);



  Shader sunShader = Shader("../shaders/sun/vertexShader.glsl", "../shaders/sun/fragShader.glsl");

  sunShader.use();
  sunShader.setVec3("sunColor", sunLight);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_callback);

  planetShader.use();
  planetShader.setVec3("sunLight", sunLight);
  planetShader.setVec3("surfaceColor", surfaceColor);
  planetShader.setFloat("ambientStrength", ambientStrength);
  planetShader.setVec3("lightPos", sunPos);
  planetShader.setFloat("specularStrength", specularStrength);

  glm::mat4 model = glm::mat4(1.0f), view = glm::mat4(1.0f), projection = glm::mat4(1.0f);


  Shader starShader = Shader("../shaders/stars/vertexShader.glsl", "../shaders/stars/fragShader.glsl");


#pragma region HDR Frame Buffer
  unsigned int hdrFBO;
  unsigned int colorBuffer;

  glGenTextures(1, &colorBuffer);
  glBindTexture(GL_TEXTURE_2D, colorBuffer);
  glGenFramebuffers(1, &hdrFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1920, 1080, 0, GL_RGBA, GL_FLOAT, nullptr);


  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);


  unsigned int rboDepth;
  glGenRenderbuffers(1, &rboDepth);
  glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1920, 1080);

  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
  glBindFramebuffer(GL_FRAMEBUFFER,0);

#pragma endregion

  unsigned int texVBO, texVAO;

  glGenBuffers(1, &texVBO);
  glGenVertexArrays(1, &texVAO);

  glBindVertexArray(texVAO);
  glBindBuffer(GL_ARRAY_BUFFER, texVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, texVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);


  Shader hdrShader = Shader("./../shaders/hdr/vertexShader.glsl", "./../shaders/hdr/fragShader.glsl");

  Shader coronaShader = Shader("./../shaders/corona/vertexShader.glsl", "./../shaders/corona/fragShader.glsl");


    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while (!glfwWindowShouldClose(window)) {
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


      planetShader.setVec3("viewPos", cam.position);

      if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
          cam.ProcessKeyboard(FORWARD, deltaTime);
      if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cam.ProcessKeyboard(BACKWARD, deltaTime);
      if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cam.ProcessKeyboard(LEFT, deltaTime);
      if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cam.ProcessKeyboard(RIGHT, deltaTime);


      projection = glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 900000.0f);


      view = cam.GetViewMatrix();
      planetShader.use();
      planetShader.setMat4("projection", projection);
      planetShader.setMat4("view", view);
      planetShader.setMat4("model", model);

      sunShader.use();
      sunShader.setMat4("view", view);
      sunShader.setMat4("projection", projection);
      sunShader.setMat4("model", model);


      starShader.use();
      starShader.setMat4("view", view);
      starShader.setMat4("projection", projection);
      starShader.setMat4("model", model);

      coronaShader.use();
      coronaShader.setMat4("view", view);
      coronaShader.setMat4("projection", projection);
      coronaShader.setMat4("model", model);
      coronaShader.setVec3("cameraPos", cam.position);

      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      planetShader.setMat4("model", model);
      glBindVertexArray(planetVAOs[0]);
      planetShader.use();
      glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(planet.triangles.size() * 3), GL_UNSIGNED_INT, nullptr);
      sunShader.use();
      glBindVertexArray(sunVAOs[0]);
      glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(sun.triangles.size() * 3), GL_UNSIGNED_INT, nullptr);

      coronaShader.use();
      glBindVertexArray(coronaVAO);
      glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(coronaSun.triangles.size() * 3), GL_UNSIGNED_INT, nullptr);


      starShader.use();
      glBindVertexArray(starsVAOs[0]);
      glDrawArrays(GL_POINTS, 0, stars.vertices.size());
      glBindFramebuffer(GL_FRAMEBUFFER,0);


      //hdrShader.use();
      //glActiveTexture(GL_TEXTURE0);
      //glBindTexture(GL_TEXTURE_2D, colorBuffer);
      //hdrShader.setInt("hdrBuffer",0);

      //glBindVertexArray(texVAO);


      glfwSwapBuffers(window);
      glfwPollEvents();
    }
}




void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = ypos - lastY;

  lastX = xpos;
  lastY = ypos;

  cam.ProcessMouseMovement(xoffset, yoffset, GL_TRUE);
}
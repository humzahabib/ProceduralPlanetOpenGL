//
// Created by hhabib on 07/07/2026.
//

#include <../include/shader.h>
#include <./../include/starMeshGenerator.h>
#include <GLFW/glfw3.h>
#include <glad.h>

#include <chrono>
#include <glm/gtc/noise.hpp>
#include <iostream>

#include "../include/camera.h"
#include "../include/icosahedronGenerator.h"
#include "./../include/SphereDeformer.h"
#include "./../include/TerrainShader.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

float sunAngle = 45.0f;
float sunRadius = 1000.0f;
glm::vec3 sunLight = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 sunPos = glm::vec3(-0.0f, 20000000.0f, 1000.0f);
float atmosphereRadius = 25000.0f;


#pragma region Camera Movement Stuff

float lastFrame = 0.0f;
float currentFrame = 0.0f;
float deltaTime;

int lastX = 400, lastY = 300;
bool firstMouse = true;

Camera cam = Camera(glm::vec3(0.0f, 0.0f, -25000.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, 0.0f);
;
void mouse_callback(GLFWwindow *window, double xpos, double ypos);

#pragma endregion

float quadVertices[] = {-1, 1, 0, 1, -1, -1, 0, 0, 1, -1, 1, 0,

                        -1, 1, 0, 1, 1,  -1, 1, 0, 1, 1,  1, 1};

int main() {
  Mesh planet, sun, stars, coronaSun;

  genIcosahedron(9000, planet);
  genIcosahedron(9000, sun);
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
    else
      starSizes.push_back(1.0f);
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

  sun.loopSubdivide(8);
  coronaSun.loopSubdivide(6);
  sun.setPosition(sunPos);
  coronaSun.setPosition(sunPos);

  planet.loopSubdivide(7);

  ApplyPerlinNoiseOnIcosphere(&planet, 300.0f, 1.0f / (900 * 2.0f), 0, false);
  planet.calculateNormals();
  ApplyPerlinNoiseOnIcosphere(&planet, 150.0f, 1.0f / (5000.0f * 1.60f), 2,
                              false);
  planet.calculateNormals();
  ApplyPerlinNoiseOnIcosphere(&planet, 20.0f, 1.0 / 25.0f, 3, false);
  planet.calculateNormals();
  std::vector<glm::vec3> colors = shade(planet);

  std::cout << "Vertices: " << planet.vertices.size() << '\n';
  std::cout << "Triangles: " << planet.triangles.size() << '\n';
  glfwInit();

#pragma region GLFW Window Stuff
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(1920, 1080, "Planet", NULL, NULL);

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_callback);

#pragma endregion

  glViewport(0, 0, 1920, 1080);

#pragma region Planet Buffers

  unsigned int planetVBOs[3], planetVAOs[1], planetEBOs[1];

  glGenBuffers(3, planetVBOs);
  glGenVertexArrays(1, planetVAOs);
  glGenBuffers(1, planetEBOs);

  // planetVAOs InitializeVAO
  glBindVertexArray(planetVAOs[0]);

  // Planet vertex data sending
  glBindBuffer(GL_ARRAY_BUFFER, planetVBOs[0]);
  glBufferData(GL_ARRAY_BUFFER, planet.vertices.size() * sizeof(glm::vec3),
               planet.vertices.data(), GL_STATIC_DRAW);
  // Planet vertex data read instructions
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // Bind planet triangle buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planetEBOs[0]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               planet.triangles.size() * sizeof(Triangle),
               planet.triangles.data(), GL_STATIC_DRAW);

  // Planet normal data sending
  glBindBuffer(GL_ARRAY_BUFFER, planetVBOs[1]);
  glBufferData(GL_ARRAY_BUFFER, planet.normals.size() * sizeof(glm::vec3),
               planet.normals.data(), GL_STATIC_DRAW);
  // Planet normal data read instructions
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);

  // Planet Color data sending
  glBindBuffer(GL_ARRAY_BUFFER, planetVBOs[2]);
  glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3),
               colors.data(), GL_STATIC_DRAW);
  // Planet color data read instructions
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(2);

#pragma endregion

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_PROGRAM_POINT_SIZE);

#pragma region Sun Buffers

  unsigned int sunVBOs[1], sunVAOs[1], sunEBOs[1];

  glGenBuffers(1, sunVBOs);
  glGenVertexArrays(1, sunVAOs);
  glGenBuffers(1, sunEBOs);

  glBindVertexArray(sunVAOs[0]);
  glBindBuffer(GL_ARRAY_BUFFER, sunVBOs[0]);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sunEBOs[0]);

  glBufferData(GL_ARRAY_BUFFER, sun.vertices.size() * sizeof(glm::vec3),
               sun.vertices.data(), GL_STATIC_DRAW);

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sun.triangles.size() * sizeof(Triangle),
               sun.triangles.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  unsigned int coronaVBO, coronaVAO, coronaEBO;
  glGenBuffers(1, &coronaVBO);
  glGenVertexArrays(1, &coronaVAO);
  glGenBuffers(1, &coronaEBO);

  glBindVertexArray(coronaVAO);
  glBindBuffer(GL_ARRAY_BUFFER, coronaVBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, coronaEBO);

  glBufferData(GL_ARRAY_BUFFER, coronaSun.vertices.size() * sizeof(glm::vec3),
               coronaSun.vertices.data(), GL_STATIC_DRAW);

  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               coronaSun.triangles.size() * sizeof(Triangle),
               coronaSun.triangles.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

#pragma endregion

#pragma region Stars Buffers
  unsigned int starsVBOs[3], starsVAOs[1];
  glGenBuffers(3, starsVBOs);
  glGenVertexArrays(1, starsVAOs);

  glBindVertexArray(starsVAOs[0]);
  glBindBuffer(GL_ARRAY_BUFFER, starsVBOs[0]);
  glBufferData(GL_ARRAY_BUFFER, stars.vertices.size() * sizeof(glm::vec3),
               stars.vertices.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, starsVBOs[1]);
  glBufferData(GL_ARRAY_BUFFER, starSizes.size() * sizeof(float),
               starSizes.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, starsVBOs[2]);
  glBufferData(GL_ARRAY_BUFFER, starColors.size() * sizeof(glm::vec3),
               starColors.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(2);

#pragma endregion

  Shader planetShader = Shader("../shaders/planet/vertexShader.glsl",
                               "../shaders/planet/fragShader.glsl");
  Shader sunShader = Shader("../shaders/sun/vertexShader.glsl",
                            "../shaders/sun/fragShader.glsl");
  Shader starShader = Shader("../shaders/stars/vertexShader.glsl",
                             "../shaders/stars/fragShader.glsl");
  Shader atmosphereShader = Shader("../shaders/atmosphere/vertexShader.glsl",
                                   "./../shaders/atmosphere/fragShader.glsl");
  Shader hdrShader = Shader("./../shaders/hdr/vertexShader.glsl",
                            "./../shaders/hdr/fragShader.glsl");
  Shader coronaShader = Shader("./../shaders/corona/vertexShader.glsl",
                               "./../shaders/corona/fragShader.glsl");

  planetShader.use();
  planetShader.setVec3("sunLight", sunLight);
  planetShader.setVec3("lightPos", sunPos);
  planetShader.setVec3("u_planetCenter", glm::vec3(0.0f, 0.0f, 0.0f));
  planetShader.setFloat("u_bottomRadius", 9000.0f);
  planetShader.setFloat("u_topRadius", atmosphereRadius);
  planetShader.setFloat("u_sunIntensity", 20.0f);
  glUniform1i(glGetUniformLocation(planetShader.ID, "u_transmittanceLUT"), 0);

  sunShader.use();
  sunShader.setVec3("sunColor", sunLight);


#pragma region HDR Frame Buffer
  unsigned int hdrFBO;
  glGenFramebuffers(1, &hdrFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

  // Texture to store image created
  unsigned int colorBuffer;
  glGenTextures(1, &colorBuffer);
  glBindTexture(GL_TEXTURE_2D, colorBuffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1920, 1080, 0, GL_RGBA, GL_FLOAT,
               nullptr);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         colorBuffer, 0);

  unsigned int depthTexture;
  glGenTextures(1, &depthTexture);
  glBindTexture(GL_TEXTURE_2D, depthTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, 1920, 1080, 0,
               GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         depthTexture, 0);

#pragma endregion

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "Frame Buffer is incomplete." << std::endl;

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  unsigned int texVBO, texVAO;

  glGenBuffers(1, &texVBO);
  glGenVertexArrays(1, &texVAO);

  glBindVertexArray(texVAO);
  glBindBuffer(GL_ARRAY_BUFFER, texVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices,
               GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, texVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices,
               GL_DYNAMIC_DRAW);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

#pragma region Transmittance Precomputations

  unsigned int transmittanceFBO;
  glGenFramebuffers(1, &transmittanceFBO);

  unsigned int transmittanceTex;
  glGenTextures(1, &transmittanceTex);
  glBindTexture(GL_TEXTURE_2D, transmittanceTex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 2048, 1024, 0, GL_RGBA, GL_FLOAT,
               NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glBindFramebuffer(GL_FRAMEBUFFER, transmittanceFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         transmittanceTex, 0);

  Shader transmittanceShader =
      Shader("./../shaders/precompute/transmittanceVertex.glsl",
             "./../shaders/precompute/transmittanceFrag.glsl");
  transmittanceShader.use();
  transmittanceShader.setFloat("bottomRadius", 9000);
  transmittanceShader.setFloat("topRadius", atmosphereRadius);

  glViewport(0, 0, 2048, 1024);
  glBindVertexArray(texVAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);

#pragma endregion

  unsigned int atmosphereFBO;
  glGenFramebuffers(1, &atmosphereFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, atmosphereFBO);

  unsigned int atmosphereColorBuffer;
  glGenTextures(1, &atmosphereColorBuffer);
  glBindTexture(GL_TEXTURE_2D, atmosphereColorBuffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1920, 1080, 0, GL_RGBA, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, atmosphereColorBuffer, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "Atmosphere Frame Buffer is incomplete." << std::endl;



  glm::mat4 model = glm::mat4(1.0f), view = glm::mat4(1.0f),
            projection = glm::mat4(1.0f);

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  while (!glfwWindowShouldClose(window)) {
    currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    sunPos = glm::vec3(cos(sunAngle) * sunRadius, sin(sunAngle) * sunRadius, 0.0f);
    sun.setPosition(sunPos);
    coronaSun.setPosition(sunPos);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      cam.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
      cam.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
      cam.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
      cam.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
      sunAngle += 1.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
      sunAngle -= 1.0f * deltaTime;

    projection = glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f,
                                  900000.0f);

    view = cam.GetViewMatrix();
    planetShader.use();

    planetShader.setVec3("u_sunDir", glm::normalize(sunPos));
    planetShader.setMat4("projection", projection);
    planetShader.setMat4("view", view);
    planetShader.setMat4("model", model);
    planetShader.setVec3("viewPos", cam.position);

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

    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glViewport(0, 0, 1920, 1080);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    planetShader.setMat4("model", model);
    glBindVertexArray(planetVAOs[0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, transmittanceTex);
    planetShader.use();

    glDrawElements(GL_TRIANGLES,
                   static_cast<GLsizei>(planet.triangles.size() * 3),
                   GL_UNSIGNED_INT, nullptr);
    sunShader.use();
    glBindVertexArray(sunVAOs[0]);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(sun.triangles.size() * 3),
                   GL_UNSIGNED_INT, nullptr);

    coronaShader.use();
    glBindVertexArray(coronaVAO);
    glDrawElements(GL_TRIANGLES,
                   static_cast<GLsizei>(coronaSun.triangles.size() * 3),
                   GL_UNSIGNED_INT, nullptr);

    starShader.use();
    glBindVertexArray(starsVAOs[0]);
    glDrawArrays(GL_POINTS, 0, stars.vertices.size());

    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    atmosphereShader.use();
    glm::mat4 inverseViewProjection = glm::inverse(projection * view);
    atmosphereShader.setMat4("u_invViewProj", inverseViewProjection);
    atmosphereShader.setVec3("u_camPos", cam.position);
    atmosphereShader.setFloat("u_width", 1920);
    atmosphereShader.setFloat("u_height", 1080);
    atmosphereShader.setVec3("u_planetCenter", glm::vec3(0.0f));
    atmosphereShader.setFloat("u_bottomRadius", 9000.0f);
    atmosphereShader.setFloat("u_topRadius", atmosphereRadius);
    atmosphereShader.setVec3("u_sunPos", sunPos);
    atmosphereShader.setInt("u_transmittanceLUT", 0);
    atmosphereShader.setInt("u_depthBuffer", 1);
    atmosphereShader.setInt("u_colorBuffer", 2);


    atmosphereShader.setVec3("u_sunLight", sunLight);
    atmosphereShader.setFloat("u_sunIntensity", 20.0f);
    atmosphereShader.setVec3("u_sunDir", glm::normalize(sunPos));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, transmittanceTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, atmosphereFBO);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    glBindVertexArray(texVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    hdrShader.use();
    glBindVertexArray(texVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, atmosphereColorBuffer);
    hdrShader.setInt("hdrBuffer", 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    glfwSwapBuffers(window);
    glfwPollEvents();

  }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
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
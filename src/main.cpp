//
// Created by hhabib on 07/07/2026.
//


//mu, mu_s, nu, r

#pragma region Includes

#include <glad.h>
#include <chrono>
#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/gtc/noise.hpp>
#include "../include/camera.h"
#include <../include/shader.h>
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "./../include/TerrainShader.h"
#include "./../include/SphereDeformer.h"
#include <./../include/starMeshGenerator.h>
#include "../include/icosahedronGenerator.h"


#pragma endregion

#pragma region Solar Params

float sunAngle = 45.0f;
float sunRadius = 40000.0f;
float sunIntensity = 20.0f;
glm::vec3 sunLight = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 sunPos = glm::vec3(-0.0f, 20000000.0f, 0.0f);

#pragma endregion

#pragma region Precompute Params

int WIDTH = 1920, HEIGHT = 1080;

int R_SIZE = 1024;
int MU_SIZE = 2048;
int MU_S_SIZE = 360;
int NU_SIZE = 360;


#pragma endregion

#pragma region Planet Params

float topRadius = 11000.0f;
float bottomRadius = 9000.0f;

glm::vec3 planetPos = glm::vec3 (0.0f, 0.0f, 15000);

#pragma endregion

#pragma region Camera and Utils Params

float deltaTime;
float lastFrame = 0.0f;
bool firstMouse = true;
float currentFrame = 0.0f;
int lastX = 400, lastY = 300;

Camera cam = Camera(glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, 0.0f);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

#pragma endregion

#pragma region FrameBuffer Params

float quadVertices[] = {-1, 1, 0, 1, -1, -1, 0, 0, 1, -1, 1, 0,

                        -1, 1, 0, 1, 1,  -1, 1, 0, 1, 1,  1, 1};

#pragma endregion


int main() {

#pragma region World Building

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
  planet.loopSubdivide(7);

  ApplyPerlinNoiseOnIcosphere(&planet, 300.0f, 1.0f / (900 * 2.0f), 0, false);
  planet.calculateNormals();
  ApplyPerlinNoiseOnIcosphere(&planet, 150.0f, 1.0f / (5000.0f * 1.60f), 2,
  false);
  planet.calculateNormals();
  ApplyPerlinNoiseOnIcosphere(&planet, 20.0f, 1.0 / 25.0f, 3, false);
  planet.calculateNormals();

  std::vector<glm::vec3> colors = shade(planet);

#pragma endregion

#pragma region GLFW Window Stuff

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Planet", NULL, NULL);

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_callback);

#pragma endregion


glViewport(0, 0, WIDTH, HEIGHT);

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

#pragma region Shaders Compilation

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

#pragma endregion

#pragma region HDR Frame Buffer

  unsigned int hdrFBO;
  glGenFramebuffers(1, &hdrFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

  // Color buffer
  unsigned int colorBuffer;
  glGenTextures(1, &colorBuffer);
  glBindTexture(GL_TEXTURE_2D, colorBuffer);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT,
               nullptr);
  // Attaching color buffer to frame buffer which is bound to HDR fbo.
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         colorBuffer, 0);

  // Depth Buffer
  unsigned int depthTexture;
  glGenTextures(1, &depthTexture);
  glBindTexture(GL_TEXTURE_2D, depthTexture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, WIDTH, HEIGHT, 0,
               GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
  // Attaching depth buffer to frame buffer which is bound to HDR fbo.
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         depthTexture, 0);


  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "Frame Buffer is incomplete." << std::endl;

#pragma endregion

  // Binding Framebuffer to monitor again since now we render on monitor.
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

#pragma region Image Plane Buffers

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

#pragma endregion

#pragma region  Precomputate Objects Generation
  // Permanent Objects
  unsigned int tFBO, eFBO, sFBO;
  unsigned int tColorTex, eColorTex, sColorTex;

  // Temporary Objects
  unsigned int delEFBO, delSFBO, delJFBO;
  unsigned int delEColorTex, delSColorTex, delJColorTex;


  // Generating T Objects
  glGenFramebuffers(1, &tFBO);
  glGenTextures(1, &tColorTex);
  glBindTexture(GL_TEXTURE_2D, tColorTex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, MU_SIZE, R_SIZE,
    0, GL_RGBA, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


  // Generating E Objects
  glGenFramebuffers(1, &eFBO);
  glGenTextures(1, &eColorTex);
  glBindTexture(GL_TEXTURE_2D, eColorTex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, MU_S_SIZE, R_SIZE,
  0, GL_RGBA, GL_FLOAT, nullptr);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


  // Generating S Objects
  glGenFramebuffers(1, &sFBO);
  glGenTextures(1, &sColorTex);
  glBindTexture(GL_TEXTURE_3D, sColorTex);
  glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA16F, MU_SIZE, MU_S_SIZE * NU_SIZE, R_SIZE,
    0, GL_RGBA, GL_FLOAT, nullptr);

  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


  // Generating delta E Objects
  glGenFramebuffers(1, &delEFBO);
  glGenTextures(1, &delEColorTex);
  glBindTexture(GL_TEXTURE_2D, delEColorTex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, MU_S_SIZE, R_SIZE,
  0, GL_RGBA, GL_FLOAT, nullptr);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  // Generating delta S Objects
  glGenFramebuffers(1, &delSFBO);
  glGenTextures(1, &delSColorTex);
  glBindTexture(GL_TEXTURE_3D, delSColorTex);
  glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA16F, MU_SIZE, MU_S_SIZE * NU_SIZE, R_SIZE,
    0, GL_RGBA, GL_FLOAT, nullptr);

  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);



  // Generating delta J Objects
  glGenFramebuffers(1, &delJFBO);
  glGenTextures(1, &delJColorTex);
  glBindTexture(GL_TEXTURE_3D, delJColorTex);
  glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA16F, MU_SIZE, MU_S_SIZE * NU_SIZE, R_SIZE,
    0, GL_RGBA, GL_FLOAT, nullptr);


  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


#pragma endregion

#pragma region Precompute Shaders Compilation

  // Pre Multi Scattering Loop Shaders
  Shader tShader = Shader("./../shaders/helpers/emptyVert.glsl",
    "./../shaders/precompute/tFrag.glsl");
  Shader delEShaderS = Shader("./../shaders/helpers/emptyVert.glsl",
    "./../shaders/precompute/single/delEFrag.glsl");
  Shader delSShaderS = Shader("./../shaders/helpers/emptyVert.glsl",
    "./../shaders/preompute/single/delSFrag");
  Shader eShader = Shader("./../shaders/helpers/emptyVert.glsl",
    "./../shaders/helpers/zeroFrag.glsl");
  Shader sShader = Shader("./../shaders/helpers/emptyVert.glsl",
    "./../shaders/precompute/sFrag.glsl");

  // In Loop Shaders
  Shader deltaJShaderM = Shader("./../shaders/helpers/emptyVert.glsl",
    "./../shaders/precompute/multi/deltaJFrag.glsl");
  Shader deltaEShaderM = Shader("./../shaders/helpers/emptyVert.glsl",
    "./../shaders/precompute/multi/deltaJFrag.glsl");
  Shader deltaSShaderM = Shader("./../shaders/helpers/emptyVert.glsl",
    "./../shaders/precompute/multi/deltaSFrag.glsl");

#pragma endregion

  glDisable(GL_BLEND);
  // T(x, v) ← T (x, ¯xo(x, v))
  glBindFramebuffer(GL_FRAMEBUFFER, tFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
    GL_TEXTURE_2D, tColorTex, 0);

  tShader.use();
  tShader.setFloat("bottomRadius", bottomRadius);
  tShader.setFloat("topRadius", topRadius);
  tShader.setInt("R_SIZE", R_SIZE);
  tShader.setInt("MU_SIZE", MU_SIZE);

  glViewport(0, 0, MU_SIZE, R_SIZE);
  glBindVertexArray(texVAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);


  // ∆E(x, s) ← ¯E[ ¯L0](x, s)
  glBindFramebuffer(GL_FRAMEBUFFER, delEFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
    GL_TEXTURE_2D, delEColorTex, 0);


  Shader firstIrradShader = Shader("./../shaders/precompute/irRadVert.glsl",
     "./../shaders/precompute/delEFrag.glsl");

  firstIrradShader.use();
  firstIrradShader.setVec3("u_sunLight", sunLight);
  firstIrradShader.setFloat("u_sunIntensity", sunIntensity);
  firstIrradShader.setFloat("u_topRadius", topRadius);
  firstIrradShader.setFloat("u_bottomRadius", bottomRadius);
  firstIrradShader.setInt("R_SIZE", R_SIZE);
  firstIrradShader.setInt("S_SIZE", MU_S_SIZE);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, transmittanceTex);
  firstIrradShader.setInt("u_transmittanceLUT", 0);

  glViewport(0, 0, MU_S_SIZE, R_SIZE);
  glBindVertexArray(texVAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);


  // ∆S(x, v, s) ← ¯S[ ¯L0](x, v, s)

  Shader firstInScatShader = Shader("./../shaders/precompute/inScatVert.glsl",
    "./../shaders/precompute/delSFrag.glsl");
  firstInScatShader.use();
  firstInScatShader.setVec3("u_sunLight", sunLight);
  firstInScatShader.setFloat("u_topRadius", topRadius);
  firstInScatShader.setFloat("u_bottomRadius", bottomRadius);
  firstInScatShader.setInt("R_SIZE", R_SIZE);
  firstInScatShader.setInt("MU_SIZE", MU_SIZE);
  firstInScatShader.setInt("MU_S_SIZE", MU_S_SIZE);
  firstInScatShader.setInt("NU_SIZE", NU_SIZE);

  unsigned int firstInScatFBO;
  unsigned int firstInScatTex;

  glGenFramebuffers(1, &firstInScatFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, firstInScatFBO);

  glGenTextures(1, &firstInScatTex);
  glBindTexture(GL_TEXTURE_3D, firstInScatTex);
  glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA16F, MU_SIZE, MU_S_SIZE * NU_SIZE, R_SIZE, 0, GL_RGBA, GL_FLOAT, nullptr);

  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


  firstInScatShader.use();
  glViewport(0, 0, MU_SIZE, MU_S_SIZE * NU_SIZE);

  for (int r = 0; r < R_SIZE; r++) {
    firstInScatShader.setInt("rLayer", r);
    glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, firstInScatTex, 0, r);
    glDrawArrays(GL_TRIANGLES, 0, 6);
  }




  // E(x, s) ← 0
  unsigned int irradianceFBO;
  unsigned int irradianceColorTex;

  glGenFramebuffers(1, &irradianceFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, irradianceFBO);

  glGenTextures(1, &irradianceColorTex);
  glBindTexture(GL_TEXTURE_2D, irradianceColorTex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, MU_S_SIZE, R_SIZE, 0, GL_RGBA, GL_FLOAT, nullptr);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, irradianceColorTex, 0);


  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  Shader zeroInitShader = Shader("./../shaders/helpers/emptyVert.glsl", "./../shaders/helpers/zeroFrag.glsl");
  zeroInitShader.use();

  glViewport(0, 0, R_SIZE, MU_S_SIZE);
  glDrawArrays(GL_TRIANGLES, 0, 6);


  // S(x, v, s) ← ∆S(x, v, s)

  unsigned int inscatterFBO;
  unsigned int inscatterColorTex;
  glGenFramebuffers(1, &inscatterFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, inscatterFBO);

  glGenTextures(1, &inscatterColorTex);
  glBindTexture(GL_TEXTURE_3D, inscatterColorTex);
  glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA16F,
    MU_SIZE, MU_S_SIZE * NU_SIZE, R_SIZE, 0,
    GL_RGBA, GL_FLOAT, nullptr);

  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  Shader inscatterShader = Shader("./../shaders/helpers/emptyVert.glsl",
    "./../shaders/precompute/sFrag.glsl");

  inscatterShader.use();
  inscatterShader.setInt("R_SIZE", R_SIZE);
  inscatterShader.setInt("MU_SIZE", MU_SIZE);
  inscatterShader.setInt("NU_SIZE", NU_SIZE);
  inscatterShader.setInt("MU_S_SIZE", MU_S_SIZE);
  inscatterShader.setFloat("u_topRadius", topRadius);
  inscatterShader.setFloat("u_bottomRadius", bottomRadius);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_3D, firstInScatTex);
  inscatterShader.setInt("scatterOrder1", 0);

  glViewport(0, 0, MU_SIZE, MU_S_SIZE * NU_SIZE);

  for (int r = 0; r < R_SIZE; r++) {
    inscatterShader.setInt("rLayer", r);
    glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, inscatterColorTex, 0, r);
    glDrawArrays(GL_TRIANGLES, 0, 6);
  }



  // ∆J(x, v, s) ← J[T·(α/π)·∆E + ∆S](x, v, s)
  // Shader deltaJShader = Shader("./../shaders/precompute/emptyVert.glsl",
  //     "./../shaders/precompute/multi/deltaJFrag.glsl");
  //
  // unsigned int deltaJFBO;
  // unsigned int deltaJTex;
  //
  // glGenFramebuffers(1, &deltaJFBO);
  // glBindFramebuffer(GL_FRAMEBUFFER, deltaJFBO);
  //
  // glGenTextures(1, &deltaJTex);
  // glBindTexture(GL_TEXTURE_3D, deltaJTex);
  // glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA16F,
  //     MU_SIZE, MU_S_SIZE * NU_SIZE, R_SIZE, 0, GL_RGBA, GL_FLOAT, nullptr);
  //
  // glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  // glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  // glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  // glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  //
  // deltaJShader.use();
  // deltaJShader.setInt("R_SIZE", R_SIZE);
  // deltaJShader.setInt("MU_SIZE", MU_SIZE);
  // deltaJShader.setInt("MU_S_SIZE", MU_S_SIZE);
  // deltaJShader.setInt("NU_SIZE", NU_SIZE);
  // deltaJShader.setFloat("u_topRadius", topRadius);
  // deltaJShader.setFloat("u_bottomRadius", bottomRadius);
  // deltaJShader.setFloat("albedo", 1.0);
  //
  // glActiveTexture(GL_TEXTURE0);
  // glBindTexture(GL_TEXTURE_2D, firstIrradianceTex);
  // deltaJShader.setInt("deltaE_prev", 0);
  //
  // glActiveTexture(GL_TEXTURE1);
  // glBindTexture(GL_TEXTURE_3D, firstInScatTex);
  // deltaJShader.setInt("deltaS_prev", 1);
  //
  // glViewport(0, 0, MU_SIZE, MU_S_SIZE * NU_SIZE);
  // glBindVertexArray(texVAO);
  //
  // for (int r = 0; r < R_SIZE; r++) {
  //   deltaJShader.setInt("rLayer", r);
  //   glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, deltaJTex, 0, r);
  //   glDrawArrays(GL_TRIANGLES, 0, 6);
  // }
  //


#pragma endregion


  unsigned int atmosphereFBO;
  glGenFramebuffers(1, &atmosphereFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, atmosphereFBO);

  unsigned int atmosphereColorBuffer;
  glGenTextures(1, &atmosphereColorBuffer);
  glBindTexture(GL_TEXTURE_2D, atmosphereColorBuffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, atmosphereColorBuffer, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "Atmosphere Frame Buffer is incomplete." << std::endl;



  glm::mat4 view = glm::mat4(1.0f),
            projection = glm::mat4(1.0f);


  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  while (!glfwWindowShouldClose(window)) {
    currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;



    sunPos = glm::vec3(0, cos(sunAngle) * sunRadius, sin(sunAngle) * sunRadius);




    planet.setPosition(planetPos - cam.position);
    sun.setPosition(sunPos - cam.position);
    coronaSun.setPosition(sunPos - cam.position);
    stars.setPosition(glm::vec3(0.0f));

    glm::mat4 sunModel = glm::translate(glm::mat4(1.0), sun.position);
    glm::mat4 planetModel = glm::translate(glm::mat4(1.0f), planet.position);

    sunShader.use();
    sunShader.setVec3("sunColor", sunLight);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      cam.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
      cam.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
      cam.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
      cam.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
      sunAngle += 0.3f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
      sunAngle -= 0.3f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_KP_ADD))
      cam.movementSpeed *= 2.0f;
    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT))
      cam.movementSpeed /= 2.0f;



    projection = glm::perspective(glm::radians(45.0f), WIDTH.0f / HEIGHT.0f, 0.1f,
                                  900000.0f);

    view = cam.GetViewMatrix();


    planetShader.use();
    glUniform1i(glGetUniformLocation(planetShader.ID, "u_transmittanceLUT"), 0);
    planetShader.setVec3("sunLight", sunLight);
    planetShader.setVec3("u_planetCenter", planet.position);
    planetShader.setFloat("u_bottomRadius", 9000.0f);
    planetShader.setFloat("u_topRadius", topRadius);
    planetShader.setFloat("u_sunIntensity", 20.0f);
    planetShader.setVec3("u_sunDir", glm::normalize(sun.position - planet.position));
    planetShader.setMat4("projection", projection);
    planetShader.setMat4("view", view);
    planetShader.setMat4("model", planetModel);
    planetShader.setVec3("viewPos", glm::vec3(0.0f));
    planetShader.setInt("u_irradianceLUT", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE, firstIrradianceTex);

    sunShader.use();
    sunShader.setMat4("model", sunModel);
    sunShader.setMat4("view", view);
    sunShader.setMat4("projection", projection);


    glm::mat4 starsModel = glm::translate(glm::mat4(1.0f), -cam.position);


    starShader.use();
    starShader.setMat4("view", view);
    starShader.setMat4("projection", projection);
    starShader.setMat4("model", starsModel);

    coronaShader.use();
    coronaShader.setMat4("view", view);
    coronaShader.setMat4("projection", projection);
    coronaShader.setMat4("model", sunModel);
    coronaShader.setVec3("cameraPos", glm::vec3(0.0f));

    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glViewport(0, 0, WIDTH, HEIGHT);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
    atmosphereShader.setVec3("u_camPos", glm::vec3(0.0));
    atmosphereShader.setFloat("u_width", WIDTH);
    atmosphereShader.setFloat("u_height", HEIGHT);
    atmosphereShader.setVec3("u_planetCenter", planet.position);
    atmosphereShader.setFloat("u_bottomRadius", 9000.0f);
    atmosphereShader.setFloat("u_topRadius", topRadius);
    atmosphereShader.setInt("u_transmittanceLUT", 0);
    atmosphereShader.setInt("u_depthBuffer", 1);
    atmosphereShader.setInt("u_colorBuffer", 2);


    atmosphereShader.setVec3("u_sunLight", sunLight);
    atmosphereShader.setFloat("u_sunIntensity", 20.0f);
    atmosphereShader.setVec3("u_sunDir", glm::normalize(sun.position - planet.position));

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
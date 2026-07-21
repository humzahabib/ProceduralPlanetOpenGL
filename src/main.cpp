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


glm::vec3 sunLight = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 sunPos = glm::vec3(-3200.0f, 3200.0f, 3200.0f);
float ambientStrength = 0.4f, specularStrength = 0.1f;
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


int main() {

  Mesh mesh;
  genIcosahedron(5000, mesh);

  //writeToFile(mesh);
  mesh.loopSubdivide(8);
  //ApplyPerlinNoiseOnIcosphere(&mesh, 800.0f, 1.0f/(900 * 1.60f), 0, true);
  //ApplyPerlinNoiseOnIcosphere(&mesh, 400.0f, 1.0f / (5000.0f * 1.60f), 2, false);
  //ApplyPerlinNoiseOnIcosphere(&mesh, 0.50f, 1.0 / 25.0f, 3);
  std::vector<glm::vec3> colors = shade(mesh);


  std::cout << "Vertices: " << mesh.vertices.size() << '\n';
  std::cout << "Triangles: " << mesh.triangles.size() << '\n';
    glfwInit();  std::vector<glm::vec3> heightNorms = shade(mesh);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "Planet", NULL, NULL);

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    glViewport(0, 0, 800 * 2, 600 * 2);


    unsigned int VBOs[3], VAO, EBO;

    glGenBuffers(3, VBOs);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(glm::vec3), mesh.vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.triangles.size() * sizeof(Triangle), mesh.triangles.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, mesh.normals.size() * sizeof(glm::vec3), mesh.normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(2);

    Shader shader = Shader("../shaders/vertexShader.glsl", "../shaders/fragShader.glsl");

    glEnable(GL_DEPTH_TEST);

    shader.use();

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_callback);

  shader.setVec3("sunLight", sunLight);
  shader.setVec3("surfaceColor", surfaceColor);
  shader.setFloat("ambientStrength", ambientStrength);
  shader.setVec3("lightPos", sunPos);
  shader.setFloat("specularStrength", specularStrength);

  glm::mat4 model = glm::mat4(1.0f), view = glm::mat4(1.0f), projection = glm::mat4(1.0f);


    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while (!glfwWindowShouldClose(window)) {
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


      shader.setVec3("viewPos", cam.position);

      if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
          cam.ProcessKeyboard(FORWARD, deltaTime);
      if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cam.ProcessKeyboard(BACKWARD, deltaTime);
      if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cam.ProcessKeyboard(LEFT, deltaTime);
      if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cam.ProcessKeyboard(RIGHT, deltaTime);


      projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 90000.0f);
      shader.setMat4("projection", projection);

      view = cam.GetViewMatrix();
      shader.setMat4("view", view);

      shader.setMat4("model", model);


        glClearColor(0.2f, 0.2f, 0.3f, 1.0f);


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(VAO);

        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.triangles.size() * 3), GL_UNSIGNED_INT, nullptr);
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
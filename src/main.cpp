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

float lastFrame = 0.0f;
float currentFrame = 0.0f;
float deltaTime;



int lastX = 400, lastY = 300;
bool firstMouse = true;

Camera cam = Camera(glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, 0.0f);;
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
int main() {

  Mesh mesh;
  genIcosahedron(1, mesh);
  mesh.loopSubdivide(2);
  std::cout << "Vertices: " << mesh.vertices.size() << '\n';
  std::cout << "Triangles: " << mesh.triangles.size() << '\n';
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "Planet", NULL, NULL);

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    glViewport(0, 0, 800, 600);


    unsigned int VBO, VAO, EBO;

    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(glm::vec3), mesh.vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.triangles.size() * sizeof(Triangle), mesh.triangles.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    Shader shader = Shader("../shaders/vertexShader.glsl", "../shaders/fragShader.glsl");

    glEnable(GL_DEPTH_TEST);

    shader.use();



    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_callback);


    glm::mat4 model = glm::mat4(1.0f), view = glm::mat4(1.0f), projection = glm::mat4(1.0f);
    int matLoc;


    while (!glfwWindowShouldClose(window)) {
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


      if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
          cam.ProcessKeyboard(FORWARD, deltaTime);
      if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cam.ProcessKeyboard(BACKWARD, deltaTime);
      if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cam.ProcessKeyboard(LEFT, deltaTime);
      if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cam.ProcessKeyboard(RIGHT, deltaTime);


      projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
      matLoc = glGetUniformLocation(shader.ID, "projection");
      glUniformMatrix4fv(matLoc, 1, GL_FALSE, glm::value_ptr(projection));

      matLoc = glGetUniformLocation(shader.ID, "view");
      view = cam.GetViewMatrix();
      glUniformMatrix4fv(matLoc, 1, GL_FALSE, glm::value_ptr(view));

      matLoc = glGetUniformLocation(shader.ID, "model");
      glUniformMatrix4fv(matLoc, 1, GL_FALSE, glm::value_ptr(model));



        glClearColor(0.2f, 0.2f, 0.3f, 1.0f);


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT);
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
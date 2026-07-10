//
// Created by hhabib on 07/07/2026.
//

#include <glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <../include/shader.h>
#include "../include/icosahedronGenerator.h"




int main() {

    float vertices[12 * 3];
    int indices[20 * 3];
  writeToFile();
  genIcosahedron(1, indices, vertices);

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

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    Shader shader = Shader("../shaders/vertexShader.glsl", "../shaders/fragShader.glsl");

    glEnable(GL_DEPTH_TEST);

    shader.use();


    while (!glfwWindowShouldClose(window)) {

        glClearColor(0.2f, 0.2f, 0.3f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(VAO);

        glDrawElements(GL_TRIANGLES,     sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
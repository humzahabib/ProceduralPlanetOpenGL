//
// Created by hhabib on 07/07/2026.
//

#include "../include/PlanetGenerator.h"
#include <iostream>

#include <GLFW/glfw3.h>

unsigned int indices[] = {
    0, 2, 1,
    0, 1, 5,
    0, 5, 4,
    0, 4, 3,
    0, 3, 2,

    11, 7, 6,
    11, 6, 10,
    11, 10, 9,
    11, 9, 8,
    11, 8, 7,

   };


int main() {
    float vertices[36];
    getIcosahedronVertices(1.0f, vertices);

    for (int i = 0; i < 12; i++)
        std::cout << vertices[i] << ", ";

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
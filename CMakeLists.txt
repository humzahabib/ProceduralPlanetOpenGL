cmake_minimum_required(VERSION 4.2)

project(ProceduralPlanetOpenGL LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 REQUIRED)

add_executable(ProceduralPlanetOpenGL
        src/main.cpp
        src/icosahedronGenerator.cpp
        src/graphics/Shader.cpp
        src/glad.c
        include/core/Camera.h
        src/core/Camera.cpp
        include/terrain/helpers.h
        include/graphics/Mesh.h
        src/graphics/Mesh.cpp
        include/SphereDeformer.h
        src/SphereDeformer.cpp
        include/TerrainShader.h
        src/TerrainShader.cpp
        include/starMeshGenerator.h
        src/starMeshGenerator.cpp
        include/core/Application.h
        include/core/Window.h
        src/core/Window.cpp
        include/renderer/Renderer.h
        src/Renderer/Renderer.cpp
        include/graphics/Framebuffer.h
        include/graphics/FullscreenQuad.h
        src/graphics/Framebuffer.cpp
        src/graphics/Framebuffer.cpp
        include/graphics/FullscreenQuad.h
        include/graphics/Framebuffer.h
        src/graphics/FullscreenQuad.cpp
        include/scene/Sun.h
        include/core/Transform.h
        src/core/Transform.cpp
        src/scene/Sun.cpp
)

target_include_directories(ProceduralPlanetOpenGL PRIVATE
        include
)

target_link_libraries(ProceduralPlanetOpenGL PRIVATE
        glfw
)
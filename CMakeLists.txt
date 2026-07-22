cmake_minimum_required(VERSION 4.2)

project(ProceduralPlanetOpenGL LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 REQUIRED)

add_executable(ProceduralPlanetOpenGL
        src/main.cpp
        src/icosahedronGenerator.cpp
        src/shader.cpp
        src/glad.c
        include/camera.h
        src/camera.cpp
        include/helpers.h
        include/Mesh.h
        src/Mesh.cpp
        include/SphereDeformer.h
        src/SphereDeformer.cpp
        include/TerrainShader.h
        src/TerrainShader.cpp
        include/starMeshGenerator.h
        src/starMeshGenerator.cpp
)

target_include_directories(ProceduralPlanetOpenGL PRIVATE
        include
)

target_link_libraries(ProceduralPlanetOpenGL PRIVATE
        glfw
)
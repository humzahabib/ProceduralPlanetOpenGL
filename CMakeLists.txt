cmake_minimum_required(VERSION 3.15)
project(ProceduralPlanetOpenGL)

project(ProceduralPlanetOpenGL LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)


# 1. Include your local GLAD and GLM folders
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# 2. Add your source files (including glad.c!)
add_executable(ProceduralPlanetOpenGL 
    src/main.cpp
    src/camera.cpp
    src/shader.cpp
    src/starMeshGenerator.cpp
    src/SphereDeformer.cpp
    src/Mesh.cpp
    src/icosahedronGenerator.cpp
    src/TerrainShader.cpp
    src/glad.c 
)

# 3. Find OpenGL on ANY operating system automatically
find_package(OpenGL REQUIRED)

# 4. Download and compile GLFW automatically
include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4 
)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# 5. Link it all together
target_link_libraries(ProceduralPlanetOpenGL 
    PRIVATE 
    glfw 
    OpenGL::GL
)
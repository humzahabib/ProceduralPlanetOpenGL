#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;

out vec3 pos;
out vec3 normal;
out vec3 surfaceColor;
uniform mat4 model, view, projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    pos = aPos;
    normal = aNormal;
    surfaceColor = aColor;
}

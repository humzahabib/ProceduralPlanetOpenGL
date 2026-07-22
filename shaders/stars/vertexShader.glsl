#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in float aSize;
layout (location = 2) in vec3 aColor;

out vec3 pos, color;
out float size;

uniform mat4 model, view, projection;


void main() {

    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    gl_PointSize = aSize;
    color = aColor;
    size = aSize;
}

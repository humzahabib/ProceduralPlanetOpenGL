#version 330 core

in vec3 pos, color;
in float size;

out vec4 FragColor;

void main() {
    FragColor = vec4(color, 1.0f);
}

#version 330


uniform vec3 sunColor;

out vec4 FragColor;


void main() {
    FragColor = vec4(sunColor, 1.0f);
}

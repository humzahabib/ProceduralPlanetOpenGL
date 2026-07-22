#version 330


uniform vec3 sunColor;

out vec4 FragColor;

vec3 sunEmission = vec3(1.0, 0.95, 0.80);

void main() {
    FragColor = vec4(sunEmission, 1.0f) * 20.0f;
}

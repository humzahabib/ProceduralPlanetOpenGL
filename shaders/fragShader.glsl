#version 330 core

out vec4 FragColor;

uniform vec3 sunLight;
uniform vec3 surfaceColor;
uniform float ambientStrength;

in vec3 pos;
in vec3 normal;

uniform vec3 lightPos;

void main() {

    // Ambient
    vec3 ambient = sunLight * ambientStrength;


    // Diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - pos);

    float diff = max(dot(norm, lightDir), 0.0);

    vec3 diffuse = diff * sunLight;

    FragColor = vec4((ambient + diffuse) * surfaceColor, 1.0f);
}
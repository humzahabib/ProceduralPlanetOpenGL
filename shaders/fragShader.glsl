#version 330 core

out vec4 FragColor;

uniform vec3 sunLight;
uniform float ambientStrength;
uniform float specularStrength;


in vec3 pos;
in vec3 normal;
in vec3 surfaceColor;

uniform vec3 lightPos, viewPos;

void main() {

    // Ambient
    vec3 ambient = sunLight * ambientStrength;


    // Diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - pos);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 diffuse = diff * sunLight;

    // Specular
    vec3 viewDir = normalize(viewPos - pos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * specularStrength * sunLight;


    FragColor = vec4((ambient + diffuse + specular) * surfaceColor, 1.0f);
}
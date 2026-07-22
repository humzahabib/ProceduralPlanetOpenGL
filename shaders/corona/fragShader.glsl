#version 330 core

in vec3 Normal;
in vec3 WorldPos;

out vec4 FragColor;

uniform vec3 cameraPos;

void main()
{
    vec3 viewDir = normalize(cameraPos - WorldPos);

    float edge =
    pow(
            1.0 - max(dot(viewDir, normalize(Normal)),0.0),
            1.0);

    vec3 color =
    vec3(1.0,0.85,0.55)
    * edge
    * 5.0;

    FragColor = vec4(color,0.0);
}
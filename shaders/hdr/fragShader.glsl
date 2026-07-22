#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;


void main () {
    vec3 hdrColor=texture(hdrBuffer,TexCoords).rgb;
    hdrColor = hdrColor / (hdrColor + vec3(1.0));
    hdrColor = pow(hdrColor, vec3(1.0/2.2));
    FragColor = vec4(hdrColor,1.0);
}
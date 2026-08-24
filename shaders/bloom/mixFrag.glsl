#version 330 core

out vec4 FragColor;

uniform sampler2D baseImage;
uniform sampler2D bloomImage;

uniform int HEIGHT, WIDTH;

void main() {
    vec2 uv = gl_FragCoord.xy / vec2(WIDTH, HEIGHT);

    FragColor = vec4(texture(baseImage, uv).rgb + texture(bloomImage, uv).rgb, 1.0);
}

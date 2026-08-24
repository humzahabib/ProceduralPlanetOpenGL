#version 330 core

vec3 luminance = vec3(0.2126, 0.7152, 0.0722);

uniform float BLOOM_THRESH;

uniform int WIDTH, HEIGHT;

uniform sampler2D hdrTexture;

void main() {
    vec2 uv = gl_FragCoord.xy / vec2(WIDTH, HEIGHT);

    vec3 baseColor = texture(hdrTexture, uv).rgb;

    float dp = dot(luminance, baseColor);

    float condition = step(BLOOM_THRESH, dp);
    vec3 newColor = mix(vec3(0.0), baseColor, condition);

    FragColor = vec4(newColor, 1.0);


}

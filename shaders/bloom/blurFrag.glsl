#version 330 core
out vec4 FragColor;
const float weights[7] = float[](
        0.0702, 0.1311, 0.1907, 0.2161, 0.1907, 0.1311, 0.0702
);
uniform bool horizontal;
uniform int WIDTH, HEIGHT;
uniform sampler2D bloomMask;
void main() {
    vec2 uv = gl_FragCoord.xy / vec2(WIDTH, HEIGHT);
    vec3 finalPixelColor = vec3(0.0);
    if (horizontal)
    {
        for (int i = 0; i < 7; i++)
        {
            int pixelIndex = i - 3;
            vec2 adjustedUV = vec2(gl_FragCoord.x + pixelIndex, gl_FragCoord.y) / vec2(WIDTH, HEIGHT);
            finalPixelColor += weights[i] * texture(bloomMask, adjustedUV).rgb;
        }
    }
    else
    {
        for (int i = 0; i < 7; i++)
        {
            int pixelIndex = i - 3;
            vec2 adjustedUV = vec2(gl_FragCoord.x, gl_FragCoord.y + pixelIndex) / vec2(WIDTH, HEIGHT);
            finalPixelColor += weights[i] * texture(bloomMask, adjustedUV).rgb;
        }
    }
    FragColor = vec4(finalPixelColor, 1.0);
}
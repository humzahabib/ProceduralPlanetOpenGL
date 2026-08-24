#version 330 core

out vec4 FragColor;

vec3 luminance = vec3(0.2126, 0.7152, 0.0722);

uniform float BLOOM_THRESH;
uniform float BLOOM_KNEE;

uniform int WIDTH, HEIGHT;

uniform sampler2D baseImage;
void main() {
    vec2 uv = gl_FragCoord.xy / vec2(WIDTH, HEIGHT);
    vec3 baseColor = texture(baseImage, uv).rgb;
    float dp = dot(luminance, baseColor);

    // soft knee: quadratic ramp around the threshold instead of a hard cutoff
    float soft = dp - BLOOM_THRESH + BLOOM_KNEE;
    soft = clamp(soft, 0.0, 2.0 * BLOOM_KNEE);
    soft = soft * soft / (4.0 * BLOOM_KNEE + 0.00001);

    float contribution = max(soft, dp - BLOOM_THRESH);
    contribution = max(contribution, 0.0);

    vec3 newColor = baseColor * (contribution / max(dp, 0.0001));
    FragColor = vec4(newColor, 1.0);
}
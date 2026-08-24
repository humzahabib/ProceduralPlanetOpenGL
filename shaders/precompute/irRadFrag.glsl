#version 330 core

out vec4 FragColor;


uniform vec3 u_sunLight;
uniform float R_SIZE, S_SIZE;
uniform float u_sunIntensity;
uniform sampler2D u_transmittanceLUT;
uniform float u_bottomRadius, u_topRadius;

vec3 getransmittanceLookUp(float mu, float r) {
    float u = (r - u_bottomRadius) / (u_topRadius - u_bottomRadius);
    float v = (mu + 1.0) / 2.0;
    vec2 tex_uv = clamp(vec2(u, v), 0.0, 1.0);

    return texture(u_transmittanceLUT, tex_uv).rgb;
}


void main() {
    vec2 uv = gl_FragCoord.xy / vec2(S_SIZE, R_SIZE);

    float mu_s = (uv.x * 2.0) - 1.0;
    float r = mix(u_bottomRadius, u_topRadius, uv.y);
    vec3 sunDir = vec3(sqrt(1 - mu_s * mu_s), mu_s, 0.0);


    vec3 centerToFrag = vec3(0.0, r, 0.0);
    vec3 zenith = normalize(centerToFrag);

    vec3 trans_sun_p = getransmittanceLookUp(mu_s, r);

    vec3 sunEnergy = u_sunIntensity * u_sunLight;
    float cosine = max(dot(zenith, sunDir), 0.0);


    FragColor = vec4(trans_sun_p * sunEnergy * cosine, 1.0f);

}

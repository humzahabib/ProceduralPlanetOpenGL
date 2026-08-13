#version 330 core

// Mapping Height (uniform)
// x -> u (v.x)
// y_1 -> u_s (s.x)
// y_2 -> nu (v.s)

uniform sampler2D transmittanceLUT;

uniform vec3 sunLight;
uniform vec3 u_sunDir;


uniform int R_SIZE, MU_SIZE, MU_S_SIZE, NU_SIZE;

uniform float u_bottomRadius;
uniform float u_topRadius;


uniform int rLayer;

vec3 betaRayleight = vec3(5.8e-3, 13.5e-3, 33.1e-3);
float betaMie = 3.9e-3;


vec2 raySphereIntersection(vec3 rayOrigin, vec3 rayDir, float radius) {
    float a = 1;
    float b = 2.0 * dot(rayOrigin, rayDir);
    float c = dot(rayOrigin, rayOrigin) - radius * radius;

    float disc = (b * b) - 4.0 * a * c;

    if (disc > 0.0f)
        return vec2((-b - sqrt(disc)) / (2 * a), (-b + sqrt(disc)) / (2 * a));
    else
        return vec2(-1.0f, 1.0f);
}


vec4 extractDimensions(vec2 coords, int layer) {
    float height = (float(layer) + 0.5) / R_SIZE;
    float mu = (float(coords.x) + 0.5) / MU_SIZE;

    int mu_s_idx = int(coords.y / MU_S_SIZE);
    int nu_idx = int(coords.y / MU_S_SIZE) + mu_s_index;

    float mu_s = (float(mu_s_idx) + 0.5) / MU_S_SIZE;
    float nu = (float(nu_idx) + 0.5) / NU_SIZE;

    return vec4(height, mu, mu_s, nu);

}

vec3 getTransmittanceLookUp(float mu, float r) {
    return texture(u_transmittanceLUT, vec2(mu, r)).rgb;
}


float rayLeightPhase(float mu) {
    return (3.0 * (1.0 + mu * mu)) / (16.0 * 3.1415);
}

float miePhae(float mu)
{
    float factor = 3.0 / (8.0 * 3.1415);
    float numerator = (1.0 - 0.76 * 0.76) * (1 + mu * mu);
    float denominator = (2.0 + 0.76 * 0.76) * pow(1.0 + 0.76 * 0.76 - 2 * 0.76 * mu, 3.0 / 2.0);

    return factor * (numerator / denominator);
}

void main() {
    // IDXs for r, mu, mu_s, nu
    vec4 texelDimensions = extractDimensions(gl_FragCoord.xy, rLayer);

    float height = (texelDimensions.x * u_topRadius) - u_bottomRadius;
    float mu = (texelDimensions.y * 2.0) - 1.0;
    float mu_s = (texelDimensions.z * 2.0) - 1.0;
    float nu = (texelDimensions.w * 2.0) - 1.0;
}

#version 330 core

out vec4 FragColor;

// From Vertex Shader
in vec3 pos;
in vec3 normal;
in vec3 surfaceColor;

// Uniforms from C++
uniform vec3 u_planetCenter;
uniform float u_bottomRadius;
uniform float u_topRadius;

uniform vec3 u_sunDir;
uniform float u_sunIntensity;
uniform vec3 sunLight; // Color of the sun
uniform vec3 viewPos;  // Camera position

uniform sampler2D u_transmittanceLUT;

vec2 getTransmittanceUV(float r, float mu) {
    float u = (mu + 1.0) / 2.0;
    float v = (r - u_bottomRadius) / (u_topRadius - u_bottomRadius);
    return clamp(vec2(u, v), 0.0, 1.0);
}

void main() {
    // 1. Calculate relative position and Zenith (straight up)
    vec3 planetToFrag = pos - u_planetCenter;
    float r = length(planetToFrag);
    vec3 zenith = planetToFrag / r;

    // 2. Sun Transmittance
    float mu_s = dot(zenith, u_sunDir);
    vec2 uv_s = getTransmittanceUV(r, mu_s);
    vec3 sunTransmittance = texture(u_transmittanceLUT, uv_s).rgb;

    // 3. Lambertian BRDF
    vec3 N = normalize(normal);
    vec3 L = normalize(u_sunDir);
    float NdotL = max(dot(N, L), 0.0);

    vec3 diffuseBRDF = surfaceColor / 3.14159265;

    // Sun Energy * Atmosphere survival * Dirt reflection * Angle
    vec3 sunEnergy = sunLight * u_sunIntensity;
    vec3 bouncedLight = (sunEnergy * sunTransmittance) * diffuseBRDF * NdotL;

    // 4. View Transmittance (Return trip to camera)
    vec3 V = normalize(viewPos - pos);
    float mu_v = dot(zenith, V);
    vec2 uv_v = getTransmittanceUV(r, mu_v);
    vec3 viewTransmittance = texture(u_transmittanceLUT, uv_v).rgb;

    vec3 finalRadiance = bouncedLight * viewTransmittance;

    FragColor = vec4(finalRadiance, 1.0);
}
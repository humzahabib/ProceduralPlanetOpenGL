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
uniform sampler2D u_irradianceLUT;

vec2 getTransmittanceUV(float mu, float r)
{
    float u = (r - u_bottomRadius) / (u_topRadius - u_bottomRadius);
    float v = (mu + 1.0) / 2.0;

    return clamp(vec2(v, u), 0.0, 1.0f);
}

void main() {
    vec3 centerToFrag = pos - u_planetCenter;
    float height = length(centerToFrag);
    vec3 zenith = centerToFrag / height;
    float mu_s = dot(zenith, u_sunDir);

    vec2 uv = vec2((mu_s + 1.0) / 2.0, height / (u_topRadius));

    vec3 irradianceP = texture(u_irradianceLUT, uv).rgb;
    vec3 brdf = surfaceColor / 3.1415;



    vec3 planetToCam = viewPos - u_planetCenter;
    vec3 camZenith = normalize(planetToCam);
    float cam_mu = dot(camZenith, normalize(viewPos - pos));
    float cam_r = clamp(length(viewPos - u_planetCenter), u_bottomRadius, u_topRadius);

    vec3 camTrans = texture(u_transmittanceLUT, getTransmittanceUV(cam_mu, cam_r)).rgb;

    float frag_mu = dot(normalize(pos - u_planetCenter), normalize(viewPos - pos));
    vec3 fragToSpaceTrans = texture(u_transmittanceLUT, getTransmittanceUV(frag_mu, height)).rgb;

    vec3 backToCamTrans = fragToSpaceTrans / max(camTrans, 0.0001f);

    FragColor = vec4(brdf * irradianceP * backToCamTrans, 1.0f);

}
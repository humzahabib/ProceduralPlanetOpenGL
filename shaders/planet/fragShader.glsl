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
    float mu = dot(zenith, u_sunDir);

    vec2 sunTransUV = getTransmittanceUV(mu, height);
    vec3 sunTransmittane = texture(u_transmittanceLUT, sunTransUV).rgb;


    vec3 sunEnergy = sunLight * u_sunIntensity;
    vec3 brdf = surfaceColor / 3.1415;
    float cosine = max(dot(normalize(normal), u_sunDir), 0.0);



    vec3 planetToCam = viewPos - u_planetCenter;
    vec3 camZenith = normalize(planetToCam);
    float cam_mu = dot(camZenith, normalize(viewPos - pos));
    float cam_r = clamp(length(viewPos - u_planetCenter), u_bottomRadius, u_topRadius);

    vec3 camTrans = texture(u_transmittanceLUT, getTransmittanceUV(cam_mu, cam_r)).rgb;
    float frag_mu = dot(zenith, normalize(viewPos - pos));
    vec3 fragToSpaceTrans = texture(u_transmittanceLUT, getTransmittanceUV(frag_mu, height)).rgb;

    vec3 backToCamTrans = fragToSpaceTrans / max(camTrans, 0.0001f);

    FragColor = vec4(sunEnergy * sunTransmittane * brdf * cosine * backToCamTrans, 1.0f);


}
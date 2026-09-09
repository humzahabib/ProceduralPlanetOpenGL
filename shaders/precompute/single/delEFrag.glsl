#version 330 core


// ∆E(x, s) ← ¯E[ ¯L0](x, s)

out vec4 FragColor;
uniform vec3 u_sunLight;
uniform int R_SIZE, MU_S_SIZE;
uniform float u_sunIntensity;
uniform sampler2D u_transmittanceLUT;
uniform float u_bottomRadius, u_topRadius;

vec3 getransmittanceLookUp(float mu, float r) {
    float u = (mu + 1.0) / 2.0;
    float v = (r - u_bottomRadius) / (u_topRadius - u_bottomRadius);
    vec2 tex_uv = clamp(vec2(u, v), 0.0, 1.0);

    return texture(u_transmittanceLUT, tex_uv).rgb;
}

void main()
{
    vec2 uv = gl_FragCoord.xy / vec2(MU_S_SIZE, R_SIZE);

    float mu_s = (uv.x * 2.0) - 1.0;
    float r = mix(u_bottomRadius, u_topRadius, uv.y);
    vec3 sunDir = vec3(sqrt(1 - mu_s * mu_s), mu_s, 0.0);


    vec3 centerToFrag = vec3(0.0, r, 0.0);
    vec3 zenith = normalize(centerToFrag);

    vec3 trans_sun_p = getransmittanceLookUp(mu_s, r);

    vec3 sunEnergy = u_sunIntensity * u_sunLight;;
    float cosine = max(dot(zenith, sunDir), 0.0);


    FragColor = vec4(sunEnergy * cosine, 1.0f);

}

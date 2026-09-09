#version 330 core

out vec4 FragColor;

float PI = 3.1415;

uniform int R_SIZE, MU_SIZE, MU_S_SIZE, NU_SIZE;
uniform float u_topRadius, u_bottomRadius, albedo;

uniform sampler3D deltaS_prev;
uniform sampler2D u_transmittanceLUT;

vec3 betaRayleigh = vec3(5.8e-3, 13.5e-3, 33.1e-3);
float betaMie = 3.9e-3;


uniform int NUM_THETA, NUM_PHI;



void main() {
    vec2 uv = gl_FragCoord.xy / vec2(MU_S_SIZE, R_SIZE);

    float mu_s = (uv.x * 2.0) - 1.0;
    float height = mix(u_bottomRadius, u_topRadius, uv.y);


    float sy = mu_s;
    float sx = sqrt(1.0 - mu_s * mu_s);
    vec3 s = vec3(sx, sy, 0.0);

    float dTheta = (PI / 2.0) / NUM_THETA;
    float dPhi = (2.0 * PI) / NUM_PHI;

    vec3 accumulatedLight = vec3(0.0);


    for (int i = 0; i < NUM_THETA; i++)
    {
        float theta = (float(i) + 0.5) * dTheta;
        for (int j = 0; j < NUM_PHI; j++)
        {
            float phi = (float(j) + 0.5) * dPhi;
            vec3 omega = vec3(cos(phi) * sin(theta), cos(theta), sin(phi) * sin(theta));
            float mu = dot(omega, vec3(0.0, 1.0, 0.0));
            float nu = dot(omega, s);

            int mu_s_idx = int(uv.x * float(MU_S_SIZE));      // uv.x IS mu_sPNormalized here already, since this whole shader's uv.x is fixed at mu_s
            int nu_idx   = int(((nu + 1.0) / 2.0) * float(NU_SIZE));
            float packedY = (float(mu_s_idx * NU_SIZE + nu_idx) + 0.5) / float(MU_S_SIZE * NU_SIZE);

            vec3 incidentLight = texture(deltaS_prev, vec3((mu + 1.0) / 2.0, packedY, uv.y)).rgb;

            // dw
            float domega = sin(theta) * dTheta * dPhi;
            accumulatedLight += incidentLight * max(0.0, mu) * domega;
        }
    }

    FragColor = vec4(accumulatedLight / PI, 1.0);

}

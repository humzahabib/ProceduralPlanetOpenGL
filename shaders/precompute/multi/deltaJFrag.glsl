#version 330 core

out vec4 FragColor;

float PI = 3.1415;
uniform int rLayer;

uniform int R_SIZE, MU_SIZE, MU_S_SIZE, NU_SIZE;
uniform float u_topRadius, u_bottomRadius, albedo;

uniform sampler2D deltaE_prev;
uniform sampler3D deltaS_prev;
uniform sampler2D u_transmittanceLUT;

vec3 betaRayleigh = vec3(5.8e-3, 13.5e-3, 33.1e-3);
float betaMie = 3.9e-3;


float rayLeighPhase(float mu) {
    return (3.0 * (1.0 + mu * mu)) / (16.0 * 3.1415);
}

float miePhase(float mu) {
    float factor = 3.0 / (8.0 * 3.1415);
    float numerator = (1.0 - 0.76 * 0.76) * (1.0 + mu * mu);
    float denominator = (2.0 + 0.76 * 0.76) * pow(1.0 + 0.76 * 0.76 - 2 * 0.76 * mu, 3.0 / 2.0);

    return factor * (numerator / denominator);
}


vec2 raySphereIntersection(vec3 origin, vec3 dir, float radius, out bool check) {
    float a = 1.0;
    float b = 2.0 * dot(origin, dir);
    float c= dot(origin, origin) - (radius * radius);

    float disc = b * b - (4.0 * a * c);
    if (disc < 0.0) {
        check = false;
        return vec2(-1.0f, -1.0);
    }

    check = true;
    return vec2((-b - sqrt(disc)) / (2.0), (-b + sqrt(disc)) / 2.0);

}


vec4 extractDimensions() {

    float height = float(rLayer + 0.5) / R_SIZE;
    float mu = (gl_FragCoord.x) / MU_SIZE;
    float mus_Idx = int(gl_FragCoord.y) / NU_SIZE;
    float nu_Idx = int(gl_FragCoord.y) % NU_SIZE;

    float mu_s = float(mus_Idx + 0.5 ) / MU_S_SIZE;
    float nu = float(nu_Idx + 0.5) / NU_SIZE;

    return vec4(mu, mu_s, nu, height);
}


void main() {
    vec4 texelDimensions = extractDimensions();

    float mu = (texelDimensions.x * 2.0) - 1.0;
    float mu_s = (texelDimensions.y * 2.0) - 1.0;
    float nu = (texelDimensions.z * 2.0) - 1.0;
    float height = mix(u_bottomRadius, u_topRadius, texelDimensions.w);

    vec3 v = vec3(sqrt(1.0 - mu * mu), mu, 0.0);

    float sy = mu_s;
    float sx = (nu - mu * sy) / sqrt(1.0 - mu * mu);
    float sz = sqrt(max(0.0, float(1.0 - sx * sx - sy * sy)));

    vec3 s = vec3(sx, sy, sz);


    const int NUM_THETA = 16;
    const int NUM_PHI = 8;


    float altitude = height - u_bottomRadius;

    float rayleighDensity = exp(-altitude / 80.0);
    float mieDensity = exp(-altitude / 12.0);

    vec3 accumulatedLight = vec3(0.0);

    for (int i = 0; i < NUM_THETA; i++)
    {
        float theta = (float(i) + 0.5) * PI / float(NUM_THETA);
        for (int j = 0; j < NUM_PHI; j++)
        {
            float phi = (float(j) + 0.5) * 2.0 * PI / float(NUM_PHI);
            vec3 omega = vec3(cos(phi) * sin(theta), cos(theta), sin(theta) * sin(phi));
            float domega = sin(theta) * (PI / NUM_THETA) * (2.0 * PI / NUM_PHI);

            float sDotOmega = dot(s, omega);
            float xDotOmega = dot(vec3(0.0, 1.0, 0.0), omega);

            float s_OmegaNorm = (sDotOmega + 1.0) / 2.0;
            float mu_OmegaNorm = (xDotOmega + 1.0) / 2.0;


            float mus_Idx_float = texelDimensions.y * float(MU_S_SIZE) - 0.5;
            float packedY = (mus_Idx_float * float(NU_SIZE) + s_OmegaNorm * float(NU_SIZE)) / float(MU_S_SIZE * NU_SIZE);
            vec3 delS = texture(deltaS_prev, vec3(mu_OmegaNorm, packedY, texelDimensions.w)).rgb;

            vec3 rayOri = vec3(0.0, height, 0.0);
            vec3 rayDir = omega;



            vec3 lightInput;

            bool groundCheck;
            vec2 groundT = raySphereIntersection(rayOri, rayDir, u_bottomRadius, groundCheck);

            vec3 delE = vec3(0.0);

            if (groundCheck && groundT.x > 0.0)
            {
                vec3 groundP = rayOri + (groundT.x * rayDir);
                vec3 localUp = normalize(groundP);
                float mu_s_g = (dot(s, localUp) + 1.0) / 2.0;
                delE = texture(deltaE_prev, vec2(mu_s_g, (length(groundP) - u_bottomRadius) / (u_topRadius - u_bottomRadius))).rgb;
            }

            float color = albedo / PI;

            vec3 transmittance = texture(u_transmittanceLUT, vec2(mu_OmegaNorm, texelDimensions.w)).rgb;

            lightInput = transmittance * color * delE + delS;


            float vDotOmega = dot(v, omega);

            float mieP = miePhase(vDotOmega);
            float rayLeighP = rayLeighPhase(vDotOmega);

            vec3 lightGathered = (betaRayleigh * rayleighDensity * rayLeighP * lightInput)
            + (betaMie * mieDensity * mieP * lightInput);
            accumulatedLight += lightGathered * domega;

        }
    }

    FragColor = vec4(accumulatedLight, 1.0f);
}

#version 330 core

out vec4 FragColor;

float PI = 3.1415;
uniform int rLayer;

uniform int R_SIZE, MU_SIZE, MU_S_SIZE, NU_SIZE;
uniform float u_topRadius, u_bottomRadius, albedo;

uniform sampler3D deltaJ;
uniform sampler2D u_transmittanceLUT;

vec3 betaRayleigh = vec3(5.8e-3, 13.5e-3, 33.1e-3);
float betaMie = 3.9e-3;


vec4 extractDimensions() {
    float muIdx = gl_FragCoord.x / float(MU_SIZE);
    float mu_sIdx = ((int(gl_FragCoord.y) / NU_SIZE) + 0.5) / MU_S_SIZE;
    float nuIdx = ((int(gl_FragCoord.y) % NU_SIZE) + 0.5) / NU_SIZE;
    float rIdx = (float(rLayer) + 0.5) / float(R_SIZE);


    return vec4(muIdx, mu_sIdx, nuIdx, rIdx);
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

void main() {
    vec4 texelDimensions = extractDimensions();



    float mu = (texelDimensions.x * 2.0) - 1.0;
    float mu_s = (texelDimensions.y * 2.0) - 1.0;
    float nu = (texelDimensions.z * 2.0) - 1.0;
    float height = mix(u_bottomRadius, u_topRadius, texelDimensions.w);

    vec3 rayOrigin = vec3(0.0, height, 0.0);
    vec3 rayDir = vec3(sqrt(1 - mu * mu), mu, 0.0);


    float sy = mu_s;
    float sx = (nu - mu * sy) / sqrt(1.0 - mu * mu);
    float sz = sqrt(max(0.0, float(1.0 - sx * sx - sy * sy)));

    vec3 s = vec3(sx, sy, sz);


    bool groundCheck;
    bool atmCheck;

    vec2 groundT = raySphereIntersection(rayOrigin, rayDir, u_bottomRadius, groundCheck);
    vec2 atmT = raySphereIntersection(rayOrigin, rayDir, u_topRadius, atmCheck);

    float terminalT = atmT.y;
    if (groundT.x > 0.0 && groundT.x < atmT.y)
        terminalT = groundT.x;


    vec3 terminalP = rayOrigin + (terminalT * rayDir);
    float distance = length(terminalP - rayOrigin);
    int numSteps = 1000;

    float stepSize = distance / numSteps;

    float currentT = 0.5;

    vec3 accumulatedLight = vec3(0.0);

    for (int i = 0; i < numSteps; i++) {
        vec3 p = rayOrigin + (currentT * rayDir);

        float heightP = length(p);
        float normalizedHeightP = (heightP - u_bottomRadius) / (u_topRadius - u_bottomRadius);
        float muP = dot(rayDir, normalize(p));
        float normalizedmuP = (muP + 1.0) / 2.0;


        float mu_sP = dot(normalize(p), s);
        float mu_sPNormalized = (mu_sP + 1.0) / 2.0;
        float nuNormalized = (dot(s, rayDir) + 1.0) / 2.0;

        float mu_s_idx = int(mu_sPNormalized * MU_S_SIZE);
        float nu_idx = int(nuNormalized * NU_SIZE);

        float combinedY = ((mu_s_idx * NU_SIZE) + nu_idx) + 0.5;
        combinedY /= float(MU_S_SIZE * NU_SIZE);

        vec3 transPtoSpace = texture(u_transmittanceLUT, vec2(normalizedmuP, normalizedHeightP)).rgb;
        vec3 transOtoSpace = texture(u_transmittanceLUT, vec2(mu, texelDimensions.w)).rgb;

        vec3 transP = transOtoSpace / transPtoSpace;

        vec3 delSValue = texture(deltaJ, vec3(normalizedmuP, combinedY, normalizedHeightP)).rgb;

        accumulatedLight += (transP * delSValue) * stepSize;
        currentT += stepSize;
    }


    FragColor = vec4(accumulatedLight, 1.0f);


}

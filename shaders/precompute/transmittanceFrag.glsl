#version 330 core

out vec4 FragColor;

uniform float bottomRadius = 9000.0;
uniform float topRadius = 9600.0;


float ray_sphere_intersect(vec3 origin, vec3 dir, float radius)
{
    float a = 1;
    float b = 2 * dot(origin, dir);
    float c = dot(origin, origin) - (radius * radius);

    float disc = (b * b) - 4 * a * c;

    if (disc > 0.0f)
    return (-b + sqrt(disc)) / 2.0;
    return -1.0f;
}

void main() {
    vec2 uv = gl_FragCoord.xy / vec2(2048.0, 1024.0);
    float mu = (uv.x * 2.0) - 1.0;
    float r = mix(bottomRadius, topRadius, uv.y);

    vec3 rayOrigin = vec3(0, r, 0);
    vec3 rayDir = vec3(sqrt(1 - mu * mu), mu, 0.0f);

    float rayLength = ray_sphere_intersect(rayOrigin, rayDir, topRadius);

    float groundCheck = ray_sphere_intersect(rayOrigin, rayDir, bottomRadius);

    if (groundCheck > 0.0f)
    rayLength = groundCheck;


    // for air molecules
    float rayleighDepth = 0.0;
    // for aerosoles
    float mieDepth = 0.0;

    int numSteps = 300;
    float dt = rayLength / numSteps;

    float t = 0.5 * dt;

    for (int i = 0; i < numSteps; i++)
    {
        vec3 samplePos = rayOrigin + (t * rayDir);
        float height = length(samplePos) - bottomRadius;
        height = max(height, 0.0);
        float rayLeightDensity = exp(-height / 80.0);
        float mieDensity = exp(-height / 12.0);

        rayleighDepth += rayLeightDensity * dt;
        mieDepth += mieDensity * dt;

        t += dt;
    }

    vec3 betaRayleight = vec3(5.8e-3, 13.5e-3, 33.1e-3);
    float betaMie = 3.9e-3;

    vec3 totalAttenuation = (betaRayleight * rayleighDepth) + (vec3(betaMie) * mieDepth);
    FragColor = vec4(exp(-totalAttenuation), 1.0f);



}
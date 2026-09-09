#version 330 core

// T(x, v) ← T (x, ¯xo(x, v))

out vec4 FragColor;


uniform int R_SIZE, MU_SIZE;
uniform float bottomRadius;
uniform float topRadius;

vec2 ray_sphere_intersect(vec3 origin, vec3 dir, float radius, out bool check)
{
    float a = 1;
    float b = 2 * dot(origin, dir);
    float c = dot(origin, origin) - (radius * radius);

    float disc = (b * b) - 4 * a * c;

    if (disc > 0.0f)
    {
        check = true;
        return vec2((-b - sqrt(disc)) / 2.0, (-b + sqrt(disc)) / 2.0);
    }
    else
    {
        check = false;
        return vec2(-1.0, -1.0);
    }
}


void main() {
    vec2 uv = gl_FragCoord.xy / vec2(float(MU_SIZE), float(R_SIZE));
    float mu = (uv.x * 2.0) - 1.0;
    float r = mix(bottomRadius, topRadius, uv.y);
    vec3 rayOrigin = vec3(0, r, 0);
    vec3 rayDir = vec3(sqrt(1.0 - mu * mu), mu, 0.0);

    bool atmCheck;
    vec2 atmT = ray_sphere_intersect(rayOrigin, rayDir, topRadius, atmCheck);

    if (atmCheck)
    {
        float rayLength = length((rayOrigin + (atmT.y * rayDir)) - rayOrigin);

        bool groundCheck;
        vec2 groundT = ray_sphere_intersect(rayOrigin, rayDir, bottomRadius, groundCheck);
        if (groundCheck && groundT.x > 0.0)
        rayLength = length((rayOrigin + (groundT.x * rayDir)) - rayOrigin);

        float rayleighDepth = 0.0;
        float mieDepth = 0.0;
        int numSteps = 700;
        float dt = rayLength / float(numSteps);
        float t = 0.5 * dt;

        for (int i = 0; i < numSteps; i++)
        {
            vec3 samplePos = rayOrigin + (t * rayDir);
            float height = length(samplePos) - bottomRadius;
            height = max(height, 0.0);
            float rayleighDensity = exp(-height / 80.0);
            float mieDensity = exp(-height / 12.0);
            rayleighDepth += rayleighDensity * dt;
            mieDepth += mieDensity * dt;
            t += dt;
        }

        vec3 betaRayleigh = vec3(5.8e-3, 13.5e-3, 33.1e-3);
        float betaMie = 3.9e-3;
        vec3 totalAttenuation = (betaRayleigh * rayleighDepth) + (vec3(betaMie) * mieDepth);
        FragColor = vec4(exp(-totalAttenuation), 1.0);
    }
    else
    FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}
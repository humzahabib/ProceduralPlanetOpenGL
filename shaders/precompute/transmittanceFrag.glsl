#version 330 core

out vec4 FragColor;

// Make sure topRadius is strictly LARGER than bottomRadius
uniform float bottomRadius = 9000.0;
uniform float topRadius = 9600.0;

// A math function to find where a ray hits a sphere
// Returns the distance to the intersection, or -1.0 if it misses
float rayIntersectSphere(vec3 ro, vec3 rd, float radius) {
    float b = dot(ro, rd);
    float c = dot(ro, ro) - radius * radius;
    float d = b * b - c;
    if (d < 0.0) return -1.0;
    return -b + sqrt(d);
}

void main() {
    // 1. Unmap UVs back to Physical Variables
    vec2 uv = gl_FragCoord.xy / vec2(256.0, 64.0);
    float mu = (uv.x * 2.0) - 1.0;
    float r = mix(bottomRadius, topRadius, uv.y);

    // 2. Set up the Ray
    vec3 rayOrigin = vec3(0.0, r, 0.0);
    // zenith is (0,1,0). The direction is based on the angle mu.
    vec3 rayDir = vec3(sqrt(1.0 - mu * mu), mu, 0.0);

    // 3. Find Ray Exit Point
    float rayLength = rayIntersectSphere(rayOrigin, rayDir, topRadius);

    // If the ray points downward and hits the dirt, we stop the ray at the ground
    float groundHit = rayIntersectSphere(rayOrigin, rayDir, bottomRadius);
    if (groundHit > 0.0) {
        rayLength = groundHit;
    }

    // 4. Raymarch to calculate Optical Depth
    int numSteps = 40; // High steps are fine here, it only runs once at startup!
    float dt = rayLength / float(numSteps);

    float opticalDepthRayleigh = 0.0;
    float opticalDepthMie = 0.0;
    float t = 0.5 * dt; // Start at a half-step for better Riemann sum accuracy

    for(int i = 0; i < numSteps; i++) {
        vec3 samplePos = rayOrigin + rayDir * t;
        float height = length(samplePos) - bottomRadius;
        height = max(height, 0.0); // Prevent negative heights

        // Atmosphere gets thinner exponentially as you go up.
        // 80.0 and 12.0 are scale heights adjusted for your 9000-unit planet.
        float densityRayleigh = exp(-height / 80.0);
        float densityMie = exp(-height / 12.0);

        opticalDepthRayleigh += densityRayleigh * dt;
        opticalDepthMie += densityMie * dt;

        t += dt;
    }

    // 5. Beer-Lambert Law (Apply scattering coefficients)
    // Blue light (z) scatters much faster than red light (x)
    vec3 betaRayleigh = vec3(5.8e-3, 13.5e-3, 33.1e-3);
    float betaMie = 3.9e-3;

    vec3 totalAttenuation = (betaRayleigh * opticalDepthRayleigh) + (vec3(betaMie) * opticalDepthMie);

    vec3 transmittance = exp(-totalAttenuation);

    FragColor = vec4(transmittance, 1.0); // Save the result to the texture
}
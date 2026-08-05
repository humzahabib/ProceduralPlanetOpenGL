#version 330 core
out vec4 FragColor;

// Uniforms to shootout rays
uniform mat4 u_invViewProj;
uniform vec3 u_camPos;
uniform float u_width, u_height;

// Uniforms to do ray calculations
uniform vec3 u_planetCenter;
uniform float u_topRadius, u_bottomRadius;
uniform sampler2D u_depthBuffer, u_transmittanceLUT;


// uniform to calculate light
uniform vec3 u_sunLight;
uniform vec3 u_sunDir;
uniform float u_sunIntensity;
uniform sampler2D u_colorBuffer;

vec3 betaRayleight = vec3(5.8e-3, 13.5e-3, 33.1e-3);
float betaMie = 3.9e-3;

in vec2 texCoords;

vec2 raySphereIntersection(vec3 rayOrigin, vec3 rayDir, float radius, vec3 center) {
    float a = 1.0;
    float b = 2.0 * dot(rayDir, rayOrigin - center);
    vec3 oc = rayOrigin - center;
    float c = dot(oc, oc) - radius * radius;

    float disc = b * b - (4 * a * c);
    float sqrtDisc = sqrt(max(disc, 0.0));
    if (disc < 0.0)
        return vec2(-1.0, -1.0);
    float t1 = (-b - sqrtDisc) / 2.0;
    float t2 = (-b + sqrtDisc) / 2.0;

    return vec2(t1, t2);

}

vec3 getWorldCoords(float x, float y, float z) {
    vec4 clipSpaceCoods = vec4((x * 2.0) - 1.0, (y * 2.0) - 1.0, (z * 2.0) - 1.0, 1.0);
    vec4 worldSpaceCoords = u_invViewProj *  clipSpaceCoods;
    worldSpaceCoords.xyz /= worldSpaceCoords.w;
    return worldSpaceCoords.xyz;
}

vec3 getTransmittanceLookUp(float angle, float height) {
    float mu = (angle + 1.0) / 2.0;
    float altitude = (height - u_bottomRadius) / (u_topRadius - u_bottomRadius);
    return texture(u_transmittanceLUT, vec2(mu, altitude)).rgb;
}

float rayleighPhase(float mu)
{
    return (3.0 * (1.0 + mu * mu)) / (16.0 * 3.1415);
}

float miePhae(float mu)
{
    float factor = 3.0 / (8.0 * 3.1415);
    float numerator = (1.0 - 0.76 * 0.76) * (1 + mu * mu);
    float denominator = (2.0 + 0.76 * 0.76) * pow(1.0 + 0.76 * 0.76 - 2 * 0.76 * mu, 3.0 / 2.0);

    return factor * (numerator / denominator);
}

void main() {

    vec3 finalColor = vec3(0.0);
    vec2 uv = gl_FragCoord.xy / vec2(u_width, u_height);

    vec3 rayOrigin = u_camPos;
    vec3 farClipPoint = getWorldCoords(uv.x, uv.y, 1.0);
    vec3 rayDir = normalize(farClipPoint - rayOrigin);

    vec3 pixelWorld = getWorldCoords(uv.x, uv.y, texture(u_depthBuffer, uv).r);

    vec2 atmCheck = raySphereIntersection(rayOrigin, rayDir, u_topRadius, u_planetCenter);
    vec2 groundCheck = raySphereIntersection(rayOrigin, rayDir, u_bottomRadius, u_planetCenter);

    float tMax = atmCheck.y;
    if (texture(u_depthBuffer, uv).r < 1.0) {
        float distanceToTerrain = length(pixelWorld - rayOrigin);
        tMax = min(tMax, distanceToTerrain);
    }

    vec3 rayTerminalPoint = rayOrigin + (tMax * rayDir);


    float renderedPixelAltitude = length(rayTerminalPoint - u_planetCenter);
    float camHeight = length(u_camPos - u_planetCenter);


    if (camHeight <= u_topRadius)
    {
        if (abs(renderedPixelAltitude - u_topRadius) < 10)
        {

            float rayLength = length(u_camPos - rayTerminalPoint);
            int numSteps = 25;
            float stepSize = rayLength / 25.0;
            float stepSizeKM = stepSize / 1000.0;
            float currentT = 0.5 * stepSize;

            vec3 accumulationOfLight = vec3(0);

            float camR = camHeight;
            float cam_mu = dot(rayDir, normalize(u_camPos - u_planetCenter));

            for (int i = 0; i < numSteps; i++)
            {
                vec3 samplePoint = rayOrigin + (currentT * rayDir);

                // checking if the sample point is lit up by the sun
                vec3 occRayOrigin = samplePoint;
                vec3 occRayDir = u_sunDir;
                vec2 groundOcc = raySphereIntersection(occRayOrigin, occRayDir, u_bottomRadius, u_planetCenter);
                vec3 lightReaching = u_sunIntensity * u_sunLight;

                if (groundOcc.x > 0.0)
                    lightReaching = vec3(0.0);


                vec3 zenithMag = samplePoint - u_planetCenter;
                float sampleAltitude = length(zenithMag) - u_bottomRadius;
                float r = length(zenithMag);
                float mu = dot(u_sunDir, normalize(zenithMag));
                vec3 transP = getTransmittanceLookUp(mu, r);

                float rayLeighDensity = exp(-sampleAltitude / 80.0);
                float mieDensity = exp(-sampleAltitude / 12.0);

                float sun_mu = dot(rayDir, -u_sunDir);

                float phaseR = rayleighPhase(sun_mu);
                float phaseM = miePhae(sun_mu);

                vec3 lightReachingP = lightReaching * transP * ((betaMie * phaseM * mieDensity) + (betaRayleight * phaseR * rayLeighDensity));

                vec3 spaceToCamTrans = getTransmittanceLookUp(cam_mu, camR);

                float mu_p = dot(rayDir, normalize(zenithMag));
                float r_p = length(zenithMag);

                vec3 spaceToPTrans = getTransmittanceLookUp(mu_p, r_p);

                vec3 transToCam = spaceToCamTrans / spaceToPTrans;

                accumulationOfLight += lightReachingP * transToCam * stepSize;

                currentT += stepSize;

            }
            finalColor = accumulationOfLight;



        }
    }


    FragColor = vec4(texture(u_colorBuffer, uv).rgb + finalColor, 1.0);




}

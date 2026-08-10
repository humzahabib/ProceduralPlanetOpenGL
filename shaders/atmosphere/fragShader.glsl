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


    float endpointT = dot(pixelWorld - rayOrigin, rayDir);
    vec3 rayTerminalPoint = rayOrigin + (endpointT * rayDir);



    float renderedPixelAltitude = length(rayTerminalPoint - u_planetCenter);
    float camHeight = length(u_camPos - u_planetCenter);

    float initialT;
    float terminalT;

    if (camHeight > u_topRadius)
    {
        if (atmCheck.x < 0.0)
        {
            FragColor = vec4(texture(u_colorBuffer, uv).rgb + finalColor, 1.0);
            return;
        }
        else
        {
            initialT = atmCheck.x;
            terminalT = atmCheck.y;

            if (groundCheck.x > 0.0)
                terminalT = groundCheck.x;
        }
    }
    else
    {
        initialT = 0.0;

        if (renderedPixelAltitude > u_topRadius)
            terminalT = atmCheck.y;
        else
            terminalT = endpointT;
    }

    vec3 initialP = rayOrigin + (initialT * rayDir);
    vec3 terminalP = rayOrigin + (terminalT * rayDir);

    float rayLength = length(initialP - terminalP);
    int numSteps = 40;
    float stepSize = rayLength / float(numSteps);
    float currentT = initialT + (0.5 * stepSize);

    vec3 accumulationOfLight = vec3(0);

    float camR = camHeight;
    float cam_mu = dot(rayDir, normalize(u_camPos - u_planetCenter));

    float viewDensityR = 0.0;
    float viewDensityM = 0.0;


    vec3 lightReaching = u_sunIntensity * u_sunLight;

    if(length(rayTerminalPoint - u_camPos) < length(u_camPos - (rayOrigin + (atmCheck.y * rayDir))))
        lightReaching = texture(u_colorBuffer, uv).rgb;

    for (int i = 0; i < numSteps; i++)
    {
        vec3 samplePoint = rayOrigin + (currentT * rayDir);

        // checking if the sample point is lit up by the sun
        vec3 occRayOrigin = samplePoint;
        vec3 occRayDir = u_sunDir;
        vec2 groundOcc = raySphereIntersection(occRayOrigin, occRayDir, u_bottomRadius, u_planetCenter);

        if (groundOcc.x > 0.0)
            lightReaching = vec3(0.0);


        vec3 zenithMag = samplePoint - u_planetCenter;
        float sampleAltitude = length(zenithMag) - u_bottomRadius;
        float r = length(zenithMag);
        float mu = dot(u_sunDir, normalize(zenithMag));
        vec3 transP = getTransmittanceLookUp(mu, r);

        float rayLeighDensity = exp(-sampleAltitude / 80.0);
        float mieDensity = exp(-sampleAltitude / 12.0);

        viewDensityM += mieDensity * stepSize;
        viewDensityR += rayLeighDensity * stepSize;

        float sun_mu = dot(rayDir, u_sunDir);

        float phaseR = rayleighPhase(sun_mu);
        float phaseM = miePhae(sun_mu);

        vec3 lightReachingP = lightReaching * transP * ((betaMie * phaseM * mieDensity) + (betaRayleight * phaseR * rayLeighDensity));

        vec3 pToCamTrans = exp(-(betaRayleight * viewDensityR + betaMie * viewDensityM));

        accumulationOfLight += lightReachingP * pToCamTrans * stepSize;

        currentT += stepSize;

    }


    finalColor = accumulationOfLight;

    vec3 viewTransmittance = exp(-(betaRayleight * viewDensityR + betaMie * viewDensityM));
    FragColor = vec4(texture(u_colorBuffer, uv).rgb * viewTransmittance + finalColor, 1.0);




}

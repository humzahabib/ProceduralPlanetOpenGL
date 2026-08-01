#version 330 core
out vec4 FragColor;

// Uniforms to shootout rays
uniform mat4 u_inverseViewProj;
uniform vec3 camPos;


// Uniforms to do ray calculations
uniform vec3 u_planetCenter;
uniform float u_topRadius, u_bottomRadius;
uniform sampler2D u_depthBuffer, u_transmittanceLUT;



in vec2 texCoords;

float raySphereIntersection(vec3 rayOrigin, vec3 rayDir, float radius, vec3 center)
{
    float a = 1.0;
    float b = 2.0 * dot(rayDir, rayOrigin - center);
    float c = dot(rayOrigin, rayOrigin) - 2 * dot(rayOrigin, center) + dot(center, center);

    float disc = pow(b, 2.0) - 4.0 * a * c;

    if (disc < 0.0)
        return -1.0;

    float t = (-b + disc) / (2.0);

    return t;
}

vec3 getWorldCoords(float x, float y)
{
    vec4 clipSpaceCoods = vec4(x, y, -1.0, 1.0);
    vec4 worldSpaceCoords = u_inverseViewProj *  clipSpaceCoods;
    worldSpaceCoords.xyz /= worldSpaceCoords.w;
    return vec3(xCam, yCam, -1.0);
}


void main() {

    float cameraAltitude = length(camPos - u_planetCenter);


    FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}

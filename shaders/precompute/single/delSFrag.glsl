#version 330 core

out vec4 FragColor;

// Mapping Height (uniform)
// r -> |x|
// x -> u (v.x)
// y_1 -> u_s (s.x)
// y_2 -> nu (v.s)


uniform int rLayer;
uniform vec3 u_sunLight;
uniform float u_topRadius;
uniform float u_bottomRadius;
uniform int R_SIZE, MU_SIZE, MU_S_SIZE, NU_SIZE;



uniform sampler2D u_transmittanceLUT;



vec2 raySphereIntersection(vec3 rayOrigin, vec3 rayDir, float radius, out bool check) {
    float a = 1;
    float b = 2.0 * dot(rayOrigin, rayDir);
    float c = dot(rayOrigin, rayOrigin) - radius * radius;

    float disc = (b * b) - 4.0 * a * c;

    if (disc > 0.0f)
    {
        check = true;
        return vec2((-b - sqrt(disc)) / (2 * a), (-b + sqrt(disc)) / (2 * a));
    }
    else
    {
        check = false;
        return vec2(-1.0f, 1.0f);
    }
}

vec4 extractDimensions(vec2 coords, int layer) {
    float height = (float(layer) + 0.5) / R_SIZE;
    float mu = (float(coords.x) + 0.5) / MU_SIZE;

    int mu_s_idx = int(coords.y / NU_SIZE);
    int nu_idx = int(coords.y) % NU_SIZE;

    float mu_s = (float(mu_s_idx) + 0.5) / MU_S_SIZE;
    float nu = (float(nu_idx) + 0.5) / NU_SIZE;

    return vec4(mu, mu_s, nu, height);

}

vec3 getTransmittanceLookUp(float mu, float r) {
    return texture(u_transmittanceLUT, vec2(mu, r)).rgb;
}

float rayLeighPhase(float mu) {
    return (3.0 * (1.0 + mu * mu)) / (16.0 * 3.1415);
}

float miePhase(float mu) {
    float factor = 3.0 / (8.0 * 3.1415);
    float numerator = (1.0 - 0.76 * 0.76) * (1 + mu * mu);
    float denominator = (2.0 + 0.76 * 0.76) * pow(1.0 + 0.76 * 0.76 - 2 * 0.76 * mu, 3.0 / 2.0);

    return factor * (numerator / denominator);
}

void main() {

    // IDXs for r, mu, mu_s, nu
    vec4 texelDimensions = extractDimensions(gl_FragCoord.xy, rLayer);

    float mu = (texelDimensions.x * 2.0) - 1.0;
    float mu_s = (texelDimensions.y * 2.0) - 1.0;
    float nu = (texelDimensions.z * 2.0) - 1.0;
    float height = u_bottomRadius + texelDimensions.w * (u_topRadius - u_bottomRadius);

    float sy = mu_s;
    float sx = (nu - mu * sy) / sqrt(1.0 - mu * mu);
    float sz = sqrt(max(0.0, 1.0 - sx * sx - sy * sy));

    vec3 sunDir = vec3(sx, sy, sz);

    vec3 rayOri = vec3(0.0f, height, 0.0f);
    vec3 rayDir = vec3(sqrt(1 - mu * mu), mu, 0.0f);

    bool groundCheck, atmCheck;
    vec2 groundT = raySphereIntersection(rayOri, rayDir, u_bottomRadius, groundCheck);
    vec2 atmT = raySphereIntersection(rayOri, rayDir, u_topRadius, atmCheck);

    vec3 terminalPoint = rayOri + (atmT.y * rayDir);

    if (atmCheck) {
        if (groundCheck) {
            if (groundT.x > 0.0 && groundT.x < atmT.y)
                terminalPoint = rayOri + (groundT.x * rayDir);
        }
    }



    float phaseR = rayLeighPhase(nu);
    float phaseM = miePhase(nu);

    float rayLength = length(terminalPoint - rayOri);
    int numSteps = 500;
    float initialT = 0.0001;
    float stepSize = rayLength / float(numSteps);
    float currentT = initialT + (0.5 * stepSize);
    vec3 accumulatedLight = vec3(0.0);


    vec3 lightReaching = u_sunLight;

    for (int i = 0; i < numSteps; i++)
    {
        vec3 samplePoint = rayOri + (currentT * rayDir);

        vec3 occRayOrigin = samplePoint;
        vec3 occRayDir = sunDir;
        bool occCheck;
        vec2 groundOcc = raySphereIntersection(occRayOrigin, occRayDir, u_bottomRadius, occCheck);

        if (occCheck && groundOcc.x > 0.0)
            lightReaching = vec3(0.0);
        else
            lightReaching = u_sunLight;

        vec3 zenithVec = samplePoint;
        float sampleAltitude = length(zenithVec) - u_bottomRadius;
        vec3 zenith = normalize(zenithVec);
        float nuP = dot(sunDir, zenith);
        vec3 trans_Space_P = getTransmittanceLookUp(nuP, sampleAltitude);

        float rayleighDensity = exp(-sampleAltitude / 80.0f);
        float mieDensity = exp(-sampleAltitude / 12.0f);



        float mu_sP = dot(rayDir, zenith);
        vec3 trans_P_Space = getTransmittanceLookUp(mu_sP, sampleAltitude);

        vec3 trans_Space_Cam = getTransmittanceLookUp(mu, height);

        vec3 trans_P_Cam = trans_Space_Cam / trans_P_Space;

        vec3 lightReadingP = lightReaching * trans_Space_P * ((betaMie * phaseM * mieDensity) + (betaRayleigh * phaseR * rayleighDensity));

        accumulatedLight +=  lightReadingP * trans_P_Cam * stepSize;

        currentT += stepSize;
    }

    FragColor = vec4(accumulatedLight, 1.0);


}

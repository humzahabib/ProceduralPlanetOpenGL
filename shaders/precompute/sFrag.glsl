#version 330 core

out vec4 FragColor;

uniform float u_topRadius, u_bottomRadius;
uniform int rLayer, R_SIZE, MU_SIZE, MU_S_SIZE, NU_SIZE;
uniform sampler3D scatterOrder1;

void main() {
    vec3 indices = vec3(gl_FragCoord.x / MU_SIZE, gl_FragCoord.y / (MU_S_SIZE * NU_SIZE), (float(rLayer) + 0.5) / R_SIZE);

    FragColor = vec4(texture(scatterOrder1, indices).rgb, 1.0);
}

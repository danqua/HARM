#version 430 core
in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vTexCoord;
out vec4 FragColor;

uniform vec4 uDiffuseColor;

void main() {
    FragColor = uDiffuseColor;
}
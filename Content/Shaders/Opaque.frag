#version 430 core
in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vTexCoord;
out vec4 FragColor;

uniform int uUseDiffuseTexture;
uniform vec4 uDiffuseColor;

uniform sampler2D uTest;
uniform sampler2D uDiffuseTexture;

void main() {
    if (uUseDiffuseTexture == 1) {
        FragColor = texture(uTest, vTexCoord);
        return;
    }
    //FragColor = uDiffuseColor;
    FragColor = vec4(1.0);
}
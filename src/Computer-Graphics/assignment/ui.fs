#version 330 core

in vec2 textureCoord;

out vec4 color;

uniform sampler2D uiTexture;

void main(void) {
    color = texture(uiTexture, textureCoord);
}
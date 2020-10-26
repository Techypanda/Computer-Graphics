#version 330 core
out vec4 FragColor;

uniform float intensity;

void main()
{
    FragColor = vec4(1.0) * intensity; // set all 4 vector values to 1.0
}

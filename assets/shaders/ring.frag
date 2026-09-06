#version 330 core

out vec4 FragColor;
uniform vec3 uColor;
in float vAlpha;

void main()
{
    FragColor = vec4(uColor, vAlpha);
}

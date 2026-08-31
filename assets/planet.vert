#version 330 core

layout(location = 0) in vec3 aPosition;

uniform mat4 uView;
uniform mat4 uProjection;

uniform vec3 uPosition;
uniform float uScale;

void main()
{
    vec3 worldPosition =
        aPosition * uScale +
        uPosition;

    gl_Position =
        uProjection *
        uView *
        vec4(worldPosition, 1.0);
}
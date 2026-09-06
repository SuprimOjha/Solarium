#version 330 core

layout(location = 0) in vec3 aPosition;

uniform mat4 uView;
uniform mat4 uProjection;

uniform vec3 uPosition;
uniform float uScale;

out vec3 vWorldPosition;
out vec3 vNormal;

void main()
{
    vec3 worldPosition =
        aPosition * uScale +
        uPosition;

    vWorldPosition = worldPosition;
    vNormal = normalize(aPosition);

    gl_Position =
        uProjection *
        uView *
        vec4(worldPosition, 1.0);
}
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in float aAlpha;

uniform mat4 uView;
uniform mat4 uProjection;
uniform vec3 uPosition;
uniform float uAxialTilt;

out float vAlpha;

void main()
{
    mat3 tilt = mat3(
        1.0, 0.0, 0.0,
        0.0, cos(uAxialTilt), -sin(uAxialTilt),
        0.0, sin(uAxialTilt), cos(uAxialTilt)
    );
    gl_Position = uProjection * uView * vec4(tilt * aPosition + uPosition, 1.0);
    vAlpha = aAlpha;
}

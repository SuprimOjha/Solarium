#version 330 core

layout(location = 0) in vec3 aPosition;

uniform mat4 uView;
uniform mat4 uProjection;

uniform vec3 uPosition;
uniform float uScale;
uniform float uRotationAngle;
uniform float uAxialTilt;

out vec3 vWorldPosition;
out vec3 vNormal;

void main()
{
    mat3 axialRotation = mat3(
        1.0, 0.0, 0.0,
        0.0, cos(uAxialTilt), -sin(uAxialTilt),
        0.0, sin(uAxialTilt), cos(uAxialTilt)
    );
    mat3 spinRotation = mat3(
        cos(uRotationAngle), 0.0, sin(uRotationAngle),
        0.0, 1.0, 0.0,
        -sin(uRotationAngle), 0.0, cos(uRotationAngle)
    );
    vec3 localPosition = axialRotation * spinRotation * aPosition;
    vec3 worldPosition =
        localPosition * uScale +
        uPosition;

    vWorldPosition = worldPosition;
    vNormal = normalize(axialRotation * spinRotation * aPosition);

    gl_Position =
        uProjection *
        uView *
        vec4(worldPosition, 1.0);
}
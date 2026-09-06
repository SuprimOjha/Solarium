#version 330 core

layout(location = 0) in vec3 aPosition;

uniform mat4 uView;
uniform mat4 uProjection;

void main()
{
    vec3 scaledPosition = aPosition / 149597870700.0;
    gl_Position =
        uProjection *
        uView *
        vec4(scaledPosition, 1.0);
}
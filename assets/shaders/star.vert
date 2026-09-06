#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in float aBrightness;
layout(location = 2) in float aTemperature;

uniform mat4 uView;
uniform mat4 uProjection;

out float vBrightness;
out float vTemperature;

void main()
{
    gl_Position = uProjection * uView * vec4(aPosition, 1.0);
    gl_PointSize = 1.0 + aBrightness * 2.0;
    vBrightness = aBrightness;
    vTemperature = aTemperature;
}

#version 330 core

out vec4 FragColor;

in float vBrightness;
in float vTemperature;

void main()
{
    vec3 cool = vec3(0.72, 0.84, 1.0);
    vec3 warm = vec3(1.0, 0.78, 0.55);
    vec3 color = mix(cool, warm, vTemperature);
    FragColor = vec4(color, vBrightness);
}

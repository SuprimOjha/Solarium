#version 330 core

out vec4 FragColor;

uniform vec3 uColor;
uniform vec3 uLightPosition;
uniform vec3 uWorldPosition;
uniform float uEmissive;

in vec3 vWorldPosition;
in vec3 vNormal;

void main()
{
    vec3 normal = normalize(vNormal);
    vec3 lightDirection = normalize(uLightPosition - vWorldPosition);
    float diffuse = max(dot(normal, lightDirection), 0.0);
    float ambient = 0.08;
    vec3 litColor = uColor * (ambient + diffuse * 0.92);
    vec3 finalColor = mix(litColor, uColor, clamp(uEmissive, 0.0, 1.0));

    FragColor = vec4(finalColor, 1.0);
}
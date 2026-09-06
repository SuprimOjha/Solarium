#version 330 core

out vec4 FragColor;

uniform vec3 uColor;
uniform vec3 uLightPosition;
uniform vec3 uWorldPosition;
uniform float uEmissive;
uniform float uSelected;
uniform float uAtmosphere;
uniform float uMaterialVariant;
uniform vec3 uCameraPosition;

in vec3 vWorldPosition;
in vec3 vNormal;

void main()
{
    vec3 normal = normalize(vNormal);
    vec3 lightDirection = normalize(uLightPosition - vWorldPosition);
    float diffuse = max(dot(normal, lightDirection), 0.0);
    float ambient = 0.08;
    vec3 litColor = uColor * (ambient + diffuse * 0.92);
    float bands = 0.5 + 0.5 * sin(vNormal.y * 18.0);
    if (uMaterialVariant > 1.5 && uMaterialVariant < 2.5) {
        litColor *= mix(0.82, 1.12, bands);
    }
    if (uMaterialVariant > 2.5) {
        float crater = 0.92 + 0.08 * sin(vNormal.x * 31.0 + vNormal.z * 19.0);
        litColor *= crater;
    }
    if (uMaterialVariant > 3.5) {
        float latitude = smoothstep(-0.25, 0.35, vNormal.y);
        float land = step(0.58, 0.5 + 0.5 * sin(vNormal.x * 7.0 + vNormal.z * 11.0));
        vec3 ocean = vec3(0.04, 0.20, 0.58);
        vec3 continent = vec3(0.16, 0.40, 0.12);
        litColor = mix(litColor, mix(ocean, continent, land), latitude * 0.65);
    }
    vec3 finalColor = mix(litColor, uColor, clamp(uEmissive, 0.0, 1.0));
    vec3 viewDirection = normalize(uCameraPosition - vWorldPosition);
    float rim = pow(1.0 - max(dot(normal, viewDirection), 0.0), 3.0);
    finalColor += uAtmosphere * rim * vec3(0.24, 0.55, 0.95);
    finalColor = mix(finalColor, finalColor + vec3(0.24, 0.42, 0.58), uSelected * 0.55);

    FragColor = vec4(finalColor, 1.0);
}
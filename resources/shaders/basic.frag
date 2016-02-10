#version 450
#extension GL_ARB_bindless_texture : require

struct materialGpuData
{
    sampler2D albedoTexture;
    sampler2D normalTexture;
    sampler2D specularTexture;
    sampler2D emissiveTexture;
    vec3 albedoColor;
    float shininess;
    vec3 specularColor;
    float reflectivity;
    vec3 emissiveColor;
    float emission;
    float opacity;
    float pad0;
    float pad1;
    float pad2;
};

layout (std140, binding = 0) buffer Materials
{
    materialGpuData items[];
} materials;

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragTangent;
in flat uint materialId;

out vec4 fragColor;

void main(void)
{
    vec3 sunPos = vec3(1.0);
    
    materialGpuData material = materials.items[materialId];

    float f = clamp(dot(sunPos, normalize(fragNormal)), 0.0, 1.0);
    fragColor = texture(material.albedoTexture, fragTexCoord) * vec4(material.albedoColor, 1.0) * f;
}

#version 450
#extension GL_ARB_bindless_texture: enable

struct materialGpuData
{
    int albedoTextureArrayIndex;
    int normalTextureArrayIndex;
    int specularTextureArrayIndex;
    int emissiveTextureArrayIndex;
    float albedoTexturePageIndex;
    float normalTexturePageIndex;
    float specularTexturePageIndex;
    float emissiveTexturePageIndex;
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

layout (std140, binding = 1) buffer Materials
{
    materialGpuData items[];
} materials;

//layout (std140, binding = 2) buffer Textures
//{
//    sampler2DArray items[];
//} textureArrays;

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragTangent;
in flat uint materialId;

out vec4 fragColor;

uniform sampler2DArray textureArrays[50];

vec4 albedo(materialGpuData material)
{
    int array = material.albedoTextureArrayIndex;
    float page = material.albedoTexturePageIndex;
    vec4 color = vec4(material.albedoColor, 1.0);
    return texture(textureArrays[array], vec3(fragTexCoord, page)) * color;
}

vec4 normal(materialGpuData material)
{
    int array = material.normalTextureArrayIndex;
    float page = material.normalTexturePageIndex;
    vec4 color = vec4(material.albedoColor, 1.0);
    return texture(textureArrays[array], vec3(fragTexCoord, page)) * color;
}

void main(void)
{
    materialGpuData material = materials.items[materialId];

    vec3 sunPos = normalize(vec3(4.0, 3.0, 2.0));
    float f = clamp(dot(sunPos, normalize(fragNormal)), 0.4, 1.0);
    fragColor = albedo(material) * f;

    //fragColor = vec4(
    //float(material.albedoTexturePageIndex)/49.0, 
    //float(material.normalTexturePageIndex)/49.0, 
    //float(material.specularTexturePageIndex)/49.0, 1.0);
}

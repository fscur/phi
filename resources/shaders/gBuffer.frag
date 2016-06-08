#version 450
#extension GL_ARB_bindless_texture: enable

struct materialRenderData
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

layout (std140, binding = 1) buffer MaterialRenderDataBuffer
{
    materialRenderData items[];
} materials;

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragTangent;
in flat uint materialId;
in flat vec4 selectionColor;

layout (location = 0) uniform sampler2DArray textureArrays[32];

layout (location = 0) out vec4 rt0;
layout (location = 1) out vec4 rt1;
layout (location = 2) out vec4 rt2;
layout (location = 3) out vec4 rt3;

vec4 fetchAlbedo(materialRenderData material)
{
    int array = material.albedoTextureArrayIndex;
    float page = material.albedoTexturePageIndex;
    vec4 color = vec4(material.albedoColor, 1.0);
    return texture(textureArrays[array], vec3(fragTexCoord, page)) * color;
}

vec4 fetchSpecular(materialRenderData material)
{
    int array = material.specularTextureArrayIndex;
    float page = material.specularTexturePageIndex;
    vec4 color = vec4(material.specularColor, 1.0);
    return texture(textureArrays[array], vec3(fragTexCoord, page)) * color;
}

vec4 fetchNormal(materialRenderData material)
{
    int array = material.normalTextureArrayIndex;
    float page = material.normalTexturePageIndex;
    return texture(textureArrays[array], vec3(fragTexCoord, page));
}

//http://aras-p.info/texts/CompactNormalStorage.html
//method 7 - stereographic projection

vec2 encodeNormal (vec3 n)
{
    float scale = 1.7777;
    vec2 enc = n.xy / (n.z + 1);
    enc /= scale;
    enc = enc * 0.5 + 0.5;
    return vec2(enc);
}

void main()
{
    materialRenderData material = materials.items[materialId];

    //normal map
    vec3 n = normalize(fragNormal);
    vec3 t = normalize(fragTangent); 
    vec3 b = cross(n, t);

    mat3 tbn = mat3(t, b, n);
    vec4 normalTexel = fetchNormal(material);
    vec3 normal = tbn * (normalTexel * 2.0 - 1.0).xyz;

    vec2 encodedNormal = encodeNormal(normal);

    vec4 albedoColor = fetchAlbedo(material);
    vec4 specularColor = fetchSpecular(material);

    rt0 = vec4(albedoColor.xyz, encodedNormal.x); //RGBA16F
    rt1 = vec4(specularColor.xyz, encodedNormal.y); //RGBA16F
    rt2 = vec4(material.shininess, 1.0, 1.0, 1.0); //RGBA16F
    rt3 = selectionColor; //RGBA8
}

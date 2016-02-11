#ifndef _PHI_MATERIAL_GPU_DATA_H_
#define _PHI_MATERIAL_GPU_DATA_H_

#include "material.h"

namespace phi
{
    struct materialGpuData
    {
    private:
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

    public:
        materialGpuData(
            textureAddress albedoTexture = textureAddress(),
            textureAddress normalTexture = textureAddress(),
            textureAddress specularTexture = textureAddress(),
            textureAddress emissiveTexture = textureAddress(),
            vec3 albedoColor = vec3(1.0f),
            vec3 specularColor = vec3(1.0f),
            vec3 emissiveColor = vec3(1.0f),
            float shininess = 0.0f,
            float reflectivity = 0.0f,
            float emission = 0.0f,
            float opacity = 1.0f) :
            albedoTextureArrayIndex(albedoTexture.arrayIndex),
            normalTextureArrayIndex(normalTexture.arrayIndex),
            specularTextureArrayIndex(specularTexture.arrayIndex),
            emissiveTextureArrayIndex(emissiveTexture.arrayIndex),
            albedoTexturePageIndex(albedoTexture.pageIndex),
            normalTexturePageIndex(normalTexture.pageIndex),
            specularTexturePageIndex(specularTexture.pageIndex),
            emissiveTexturePageIndex(emissiveTexture.pageIndex),
            albedoColor(albedoColor),
            specularColor(specularColor),
            emissiveColor(emissiveColor),
            shininess(shininess),
            reflectivity(reflectivity),
            emission(emission),
            opacity(1.0f),
            pad0(0.0f),
            pad1(0.0f),
            pad2(0.0f)
        {}
    };
}
#endif
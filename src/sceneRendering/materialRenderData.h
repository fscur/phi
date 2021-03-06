#pragma once
#include <phi.h>
#include <core\material.h>
#include <rendering\textureAddress.h>

namespace phi
{
    struct materialRenderData
    {
    private:
        int albedoTextureUnit;
        int normalTextureUnit;
        int specularTextureUnit;
        int emissiveTextureUnit;
        float albedoTexturePage;
        float normalTexturePage;
        float specularTexturePage;
        float emissiveTexturePage;
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
        materialRenderData(
            int albedoTextureUnit = 0,
            int normalTextureUnit = 0,
            int specularTextureUnit = 0,
            int emissiveTextureUnit = 0,
            float albedoTexturePage = 0.0f,
            float normalTexturePage = 0.0f,
            float specularTexturePage = 0.0f,
            float emissiveTexturePage = 0.0f,
            vec3 albedoColor = vec3(1.0f),
            vec3 specularColor = vec3(1.0f),
            vec3 emissiveColor = vec3(1.0f),
            float shininess = 0.0f,
            float reflectivity = 0.0f,
            float emission = 0.0f,
            float opacity = 1.0f) :
            albedoTextureUnit(albedoTextureUnit),
            normalTextureUnit(normalTextureUnit),
            specularTextureUnit(specularTextureUnit),
            emissiveTextureUnit(emissiveTextureUnit),
            albedoTexturePage(albedoTexturePage),
            normalTexturePage(normalTexturePage),
            specularTexturePage(specularTexturePage),
            emissiveTexturePage(emissiveTexturePage),
            albedoColor(albedoColor),
            shininess(shininess),
            specularColor(specularColor),
            reflectivity(reflectivity),
            emissiveColor(emissiveColor),
            emission(emission),
            opacity(opacity),
            pad0(0.0f),
            pad1(0.0f),
            pad2(0.0f)
        {
        }

        materialRenderData(
            const textureAddress& albedoTextureAddress,
            const textureAddress& normalTextureAddress,
            const textureAddress& specularTextureAddress,
            const textureAddress& emissiveTextureAddress,
            const material* const material)
            : materialRenderData::materialRenderData(
                albedoTextureAddress.index,
                normalTextureAddress.index,
                specularTextureAddress.index,
                emissiveTextureAddress.index,
                albedoTextureAddress.page,
                normalTextureAddress.page,
                specularTextureAddress.page,
                emissiveTextureAddress.page,
                material->albedoColor,
                material->specularColor,
                material->emissiveColor,
                material->shininess,
                material->reflectivity,
                material->emission,
                material->opacity)
        {
        }
    };
}
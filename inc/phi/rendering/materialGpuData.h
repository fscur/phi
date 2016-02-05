#ifndef _PHI_MATERIAL_GPU_DATA_H_
#define _PHI_MATERIAL_GPU_DATA_H_

#include "material.h"

namespace phi
{
    struct materialGpuData
    {
    private:
        GLuint64 albedoTexture;
        GLuint64 normalTexture;
        GLuint64 specularTexture;
        GLuint64 emissiveTexture;
        vec3 albedoColor;
        vec3 specularColor;
        vec3 emissiveColor;
        float shininess;
        float reflectivity;
        float emission;
        float opacity;
        float pad0;
        float pad1;
        float pad2;

    public:
        materialGpuData(
            GLuint64 albedoTexture = 0,
            GLuint64 normalTexture = 0,
            GLuint64 specularTexture = 0,
            GLuint64 emissiveTexture = 0,
            vec3 albedoColor = vec3(1.0f),
            vec3 specularColor = vec3(1.0f),
            vec3 emissiveColor = vec3(1.0f),
            float shininess = 0.0f,
            float reflectivity = 0.0f,
            float emission = 0.0f,
            float opacity = 1.0f) :
            albedoTexture(albedoTexture),
            normalTexture(normalTexture),
            specularTexture(specularTexture),
            emissiveTexture(emissiveTexture),
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

        static materialGpuData fromMaterial(phi::material material)
        {
            return materialGpuData(
                material.albedoTexture->handle,
                material.normalTexture->handle,
                material.specularTexture->handle,
                material.emissiveTexture->handle,
                material.albedoColor,
                material.specularColor,
                material.emissiveColor,
                material.shininess,
                material.reflectivity,
                material.emission,
                material.opacity);
        }
    };
}
#endif
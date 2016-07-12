#pragma once
#include <phi.h>
#include "coreApi.h"
#include "image.h"

namespace phi
{
    struct material
    {
    public:
        CORE_API static material* defaultMaterial;

    public:
        image* albedoImage;
        image* normalImage;
        image* specularImage;
        image* emissiveImage;
        vec3 albedoColor;
        vec3 specularColor;
        vec3 emissiveColor;
        float shininess;
        float reflectivity;
        float emission;
        float opacity;

    public:
        material(
            image* albedoImage,
            image* normalImage,
            image* specularImage,
            image* emissiveImage,
            vec3 albedoColor = vec3(1.0f),
            vec3 specularColor = vec3(1.0f),
            vec3 emissiveColor = vec3(1.0f),
            float shininess = 0.0f,
            float reflectivity = 0.0f,
            float emission = 0.0f,
            float opacity = 1.0f) :
            albedoImage(albedoImage),
            normalImage(normalImage),
            specularImage(specularImage),
            emissiveImage(emissiveImage),
            albedoColor(albedoColor),
            specularColor(specularColor),
            emissiveColor(emissiveColor),
            shininess(shininess),
            reflectivity(reflectivity),
            emission(emission),
            opacity(1.0f)
        {}

    private: 
        static material* createDefaultMaterial();

    public:
        CORE_API static void release();
    };
}
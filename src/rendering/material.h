#pragma once

#include "texture.h"

namespace phi
{
    struct material
    {
    public:
        texture* albedoTexture;
        texture* normalTexture;
        texture* specularTexture;
        texture* emissiveTexture;
        vec3 albedoColor;
        vec3 specularColor;
        vec3 emissiveColor;
        float shininess;
        float reflectivity;
        float emission;
        float opacity;

    public:
        RENDERING_API material(
            texture* albedoTexture,
            texture* normalTexture,
            texture* specularTexture,
            texture* emissiveTexture,
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
            opacity(1.0f)
        {}
    };
}
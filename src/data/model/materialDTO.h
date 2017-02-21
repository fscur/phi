#pragma once
#include <phi.h>

#include <core/guid.h>
#include <core/material.h>

#include "vec3DTO.h"

namespace phi
{
    struct materialDTO
    {

    public:
        materialDTO(){}
    
        materialDTO(phi::guid guid,
            phi::guid albedoTextureGuid,
            phi::guid normalTextureGuid,
            phi::guid specularTextureGuid,
            phi::guid emissiveTextureGuid,
            vec3 albedoColor,
            vec3 specularColor,
            vec3 emissiveColor,
            float shininess,
            float reflectivity,
            float emission,
            float opacity) :
            albedoTextureGuid(albedoTextureGuid),
            normalTextureGuid(normalTextureGuid),
            specularTextureGuid(specularTextureGuid),
            emissiveTextureGuid(emissiveTextureGuid),
            albedoColor(albedoColor),
            specularColor(specularColor),
            emissiveColor(emissiveColor),
            shininess(shininess),
            reflectivity(reflectivity),
            emission(emission),
            opacity(opacity)
        {
        }

        static materialDTO from(phi::material* material)
        {
            return materialDTO(
                material->getGuid(),
                material->albedoImage->getGuid(),
                material->normalImage->getGuid(),
                material->specularImage->getGuid(),
                material->emissiveImage->getGuid(),
                material->albedoColor,
                material->specularColor,
                material->emissiveColor,
                material->shininess,
                material->reflectivity,
                material->emission,
                material->opacity);
        }

        template <typename Archive>
        void serialize(Archive& archive)
        {
            archive(
                cereal::make_nvp("Guid", guid),
                cereal::make_nvp("AlbedoTextureGuid", albedoTextureGuid),
                cereal::make_nvp("NormalTextureGuid", normalTextureGuid),
                cereal::make_nvp("SpecularTextureGuid", specularTextureGuid),
                cereal::make_nvp("EmissiveTextureGuid", emissiveTextureGuid),
                cereal::make_nvp("AlbedoColor", albedoColor.vector),
                cereal::make_nvp("SpecularColor", specularColor.vector),
                cereal::make_nvp("EmissiveColor", emissiveColor.vector),
                cereal::make_nvp("Shininess", shininess),
                cereal::make_nvp("Reflectivity", reflectivity),
                cereal::make_nvp("Emission", emission),
                cereal::make_nvp("Opacity", opacity)
            );
        }

    public:
        phi::guid guid;
        phi::guid albedoTextureGuid;
        phi::guid normalTextureGuid;
        phi::guid specularTextureGuid;
        phi::guid emissiveTextureGuid;
        vec3DTO albedoColor;
        vec3DTO specularColor;
        vec3DTO emissiveColor;
        float shininess;
        float reflectivity;
        float emission;
        float opacity;
    };
}
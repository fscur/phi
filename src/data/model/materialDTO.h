#pragma once
#include <phi.h>

#include <core/guid.h>

#include "vec3DTO.h"

namespace phi
{
    struct materialDTO
    {
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
    };
}
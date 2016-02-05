#ifndef _PHI_MATERIAL_H_
#define _PHI_MATERIAL_H_

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
        float pad0;
        float pad1;
        float pad2;

    private:
        static material* _default;

    public:
        
        RENDERING_API static material* material::default()
        {
            if (_default == nullptr)
                _default = new material(
                    texture::defaultAlbedo(),
                    texture::defaultNormal(),
                    texture::defaultSpecular(),
                    texture::defaultEmissive(),
                    vec3(1.0f),
                    vec3(1.0f),
                    vec3(1.0f),
                    0.0f,
                    0.0f,
                    0.0f,
                    1.0f);

            return _default;
        }

    public:
        RENDERING_API material(
            texture* albedoTexture,
            texture* normalTexture,
            texture* specularTexture,
            texture* emissiveTexture,
            vec3 diffuseColor = vec3(1.0f),
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

        static material* material::lambertian(vec3 color)
        {
            return new material(
                texture::defaultAlbedo(),
                texture::defaultNormal(),
                texture::defaultSpecular(),
                texture::defaultEmissive(),
                vec3(color),
                vec3(color),
                vec3(color),
                0.0f,
                0.0f,
                0.0f,
                1.0f);
        }
    };
}
#endif
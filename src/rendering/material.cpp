#include <phi/io/path.h>
#include <phi/rendering/renderingSystem.h>
#include <phi/rendering/material.h>

#include <fstream>

namespace phi
{
    material* material::defaultMaterial = nullptr;

    material* material::getDefault()
    {
        if (defaultMaterial == nullptr)
            defaultMaterial = new material(
                "default",
                "",
                texture::getDefaultDiffuse(),
                texture::getDefaultNormal(),
                texture::getDefaultSpecular(),
                texture::getDefaultEmissive(),
                color::white,
                color::white,
                color::white,
                color::white,
                0.2f,
                0.8f,
                0.0f,
                0.0f,
                0.0f,
                false,
                nullptr);
        
        return defaultMaterial;
    }

    material* material::getLambert(color color)
    {
        return new material(
                "lambert",
                "",
                texture::getDefaultDiffuse(),
                texture::getDefaultNormal(),
                texture::getDefaultSpecular(),
                texture::getDefaultEmissive(),
                color,
                color,
                color,
                color,
                0.2f,
                0.8f,
                0.0f,
                0.0f,
                0.0f,
                false,
                nullptr);
    }

    material::material(
        texture* diffuseTexture,
        texture* normalTexture,
        texture* specularTexture, 
        texture* emissiveTexture, 
        color ambientColor,
        color diffuseColor, 
        color specularColor, 
        color emissiveColor,
        float ka,
        float kd,
        float ks,
        float shininess,
        float reflectivity,
        bool isEmissive)
    {
        _diffuseTexture = diffuseTexture;
        _normalTexture = normalTexture;
        _specularTexture = specularTexture;
        _emissiveTexture = emissiveTexture;
        _ambientColor = ambientColor;
        _diffuseColor = diffuseColor;
        _specularColor = specularColor; 
        _emissiveColor = emissiveColor;
        _ka = ka;
        _kd = kd;
        _ks = ks;
        _shininess = shininess;
        _reflectivity = reflectivity;
        _isEmissive = isEmissive;
    }

    material::material(
        std::string diffuseTextureName,
        std::string normalTextureName,
        std::string specularTextureName,
        std::string emissiveTextureName, 
        color ambientColor,
        color diffuseColor,
        color specularColor,
        color emissiveColor,
        float ka,
        float kd,
        float ks,
        float shininess,
        float reflectivity,
        bool isEmissive)
    {
        _diffuseTextureName = diffuseTextureName;
        _normalTextureName = normalTextureName;
        _specularTextureName = specularTextureName;
        _emissiveTextureName = emissiveTextureName;
        _ambientColor = ambientColor;
        _diffuseColor = diffuseColor;
        _specularColor = specularColor; 
        _emissiveColor = emissiveColor;
        _ka = ka;
        _kd = kd;
        _ks = ks;
        _shininess = shininess;
        _reflectivity = reflectivity;
        _isEmissive = isEmissive;
    }

    material::~material()
    {
    }
}
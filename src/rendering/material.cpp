#include <phi/io/path.h>
#include <phi/rendering/renderingSystem.h>
#include <phi/rendering/material.h>

#include <fstream>

namespace phi
{
    material* material::default = new material(
        texture::defaultDiffuse,
        texture::defaultNormal,
        texture::defaultSpecular,
        texture::defaultEmissive,
        color::white,
        color::white,
        color::white,
        color::white,
        0.2f,
        0.8f,
        0.0f,
        0.0f,
        0.0f,
        false);

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
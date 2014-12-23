#include "Material.h"
#include "ResourceManager.h"

namespace phi
{
    material::material(
        texture* diffuseTexture, 
        texture* normalTexture, 
        texture* specularTexture,
        color ambientColor,
        color diffuseColor, 
        color specularColor, 
        float ka,
        float kd,
        float ks,
        float shininess)
    {
        if (diffuseTexture == nullptr)
            _diffuseTexture = resourceManager::get()->getTexture("default_diffuseMap");
        else
            _diffuseTexture = diffuseTexture;

        if (normalTexture == nullptr)
            _normalTexture = resourceManager::get()->getTexture("default_normalMap");
        else
		    _normalTexture = normalTexture;

        if (specularTexture == nullptr)
            _specularTexture = resourceManager::get()->getTexture("default_specularMap");
        else
		    _specularTexture = specularTexture;

        _ambientColor = ambientColor;
        _diffuseColor = diffuseColor;
        _specularColor = specularColor; 
        _ka = ka;
        _kd = kd;
        _ks = ks;
        _shininess = shininess;
    }

    material::~material()
    {}
}
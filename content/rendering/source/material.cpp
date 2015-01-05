#include "material.h"
#include "renderingSystem.h"

namespace phi
{
    material::material(
        std::string name,
        std::string path,
        texture* diffuseTexture, 
        texture* normalTexture, 
        texture* specularTexture,
        color ambientColor,
        color diffuseColor, 
        color specularColor, 
        float ka,
        float kd,
        float ks,
        float shininess) :
        resource(name, path)
    {
        if (diffuseTexture == nullptr)
            _diffuseTexture = renderingSystem::repository->getResource<texture>("default_diffuseMap");
        else
            _diffuseTexture = diffuseTexture;

        if (normalTexture == nullptr)
            _normalTexture = renderingSystem::repository->getResource<texture>("default_normalMap");
        else
            _normalTexture = normalTexture;

        if (specularTexture == nullptr)
            _specularTexture = renderingSystem::repository->getResource<texture>("default_specularMap");
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
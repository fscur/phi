#include "globals.h"
#include "rendering.h"
#include "texture.h"
#include "material.h"
#include "mesh.h"

namespace phi
{
    resourcesRepository* rendering::repository;
    bool rendering::initialized = false;

    void rendering::init(std::string path)
    {
        repository = new resourcesRepository();
        
        initTextures(path);
        initMaterials(path);
        initMeshes(path);

        initialized = true;
    }

    void rendering::initTextures(std::string path)
    {
        std::string texturesPath = path + RENDERING_TEXTURES_PATH;
        repository->addResource(texture::fromFile(texturesPath + "default_diffuseMap.bmp"));
        repository->addResource(texture::fromFile(texturesPath + "default_normalMap.bmp"));
        repository->addResource(texture::fromFile(texturesPath + "default_specularMap.bmp"));
        repository->addResource(texture::fromFile(texturesPath + "alpha.bmp"));
        repository->addResource(texture::fromFile(texturesPath + "close.bmp"));
        repository->addResource(texture::fromFile(texturesPath + "test.tga"));
        repository->addResource(texture::fromFile(texturesPath + "diffuse.bmp"));
        repository->addResource(texture::fromFile(texturesPath + "normal.bmp"));
        repository->addResource(texture::fromFile(texturesPath + "specular.bmp"));
        repository->addResource(texture::fromFile(texturesPath + "BubbleGrip-ColorMap.bmp"));
        repository->addResource(texture::fromFile(texturesPath + "BubbleGrip-NormalMap.bmp"));
    }

    void rendering::initMaterials(std::string path)
    {
        repository->addResource(new material("bricks", "",
            repository->getResource<texture>("diffuse"), //diffuse texture
            repository->getResource<texture>("normal"), //normal texture
            repository->getResource<texture>("specular"), //specular texture,
            color::white, // ambient color
            color::white, //diffuse color
            color::gray, //specular color
            0.2f, // ambient reflectance coeff
            0.8f, // diffuse reflectance coeff
            0.1f, // specular reflectance coeff
            6.5f)); // shininess

        repository->addResource(new material("red", "",
            nullptr, //diffuse texture
            nullptr, //normal texture
            nullptr, //specular texture,
            color::red, // ambient color
            color::red, //diffuse color
            color::red, //specular color
            0.2f, // ambient reflectance coeff
            1.0f, // diffuse reflectance coeff
            0.0f, // specular reflectance coeff
            0.0f)); // shininess

        repository->addResource(new material("white", "",
            nullptr, //diffuse texture
            nullptr, //normal texture
            nullptr, //specular texture,
            color::white, // ambient color
            color::white, //diffuse color
            color::white, //specular color
            0.1f, // ambient reflectance coeff
            0.8f, // diffuse reflectance coeff
            0.3f, // specular reflectance coeff
            64.0f)); // shininess

        repository->addResource(new material("bubble", "",
            repository->getResource<texture>("BubbleGrip-ColorMap"), //diffuse texture
            repository->getResource<texture>("BubbleGrip-NormalMap"), //normal texture
            nullptr, //specular texture,
            color::red, // ambient color
            color::orange, //diffuse color
            color::orange, //specular color
            0.2f, // ambient reflectance coeff
            0.3f, // diffuse reflectance coeff
            4.8f, // specular reflectance coeff
            8.0f)); // shininess
    }

    void rendering::initMeshes(std::string path)
    {
        std::string meshesPath = path + RENDERING_MESHES_PATH;
        repository->addResource(mesh::fromObj("box", meshesPath + "cube.model"));
    }

    void rendering::release()
    {
        repository->release();
        DELETE(repository);
    }
}
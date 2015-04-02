#include "resourcesLoader.h"
#include "path.h"
#include "model.h"
#include "fileInfo.h"
#include "directoryInfo.h"
#include "renderingSystem.h"

namespace phi
{
    resourcesLoader::resourcesLoader()
    {
        _texturesRepository = nullptr;
        _materialsRepository = nullptr;
        _modelsRepository = nullptr;

        _texturesPath = "resources\\textures";
        _materialsPath = "resources\\materials";
        _modelsPath = "resources\\models";

        createDefaultTextures();
        createDefaultMaterial();
    }

    void resourcesLoader::createDefaultTextures()
    {
        char* data = new char[4];
        data[0] = 255;
        data[1] = 255;
        data[2] = 255;
        data[3] = 255;

        _defaultDiffuseTexture = 
            texture::create(size<GLuint>(1, 1), GL_RGBA, GL_BGRA, GL_UNSIGNED_BYTE, 0, data);

        _defaultSpecularTexture = 
            texture::create(size<GLuint>(1, 1), GL_RGBA, GL_BGRA, GL_UNSIGNED_BYTE, 0, data);

        _defaultEmissiveTexture = 
            texture::create(size<GLuint>(1, 1), GL_RGBA, GL_BGRA, GL_UNSIGNED_BYTE, 0, data);

        data[0] = 255;
        data[1] = 128;
        data[2] = 128;
        data[3] = 255;

        _defaultNormalTexture = 
            texture::create(size<GLuint>(1, 1), GL_RGBA, GL_BGRA, GL_UNSIGNED_BYTE, 0, data);
    }

    void resourcesLoader::createDefaultMaterial()
    {
        _defaultMaterial = new material(
            "default",
            "",
            _defaultDiffuseTexture,
            _defaultNormalTexture,
            _defaultSpecularTexture,
            _defaultEmissiveTexture,
            color::white,
            color::white,
            color::white,
            color::white,
            0.2f,
            0.8f,
            0.0f,
            0.0f,
            false);
    }

    std::string resourcesLoader::getFullName(std::string relativePath, std::string fileName)
    {
        return fileName.substr(relativePath.size() + 1, fileName.size() - 1);
    }

    void resourcesLoader::loadTextures()
    {
        _texturesRepository = new resourcesRepository();

        loadTextures(_texturesPath);
    }

    void resourcesLoader::loadTextures(std::string directory)
    {
        auto subDirs = path::getDirectories(directory);
        auto subDirsCount = subDirs.size();

        for (auto i = 0; i < subDirsCount; i++)
            loadTextures(subDirs[i].path);

        auto files = path::getFiles(directory);

        auto filesCount = files.size();

        for (auto i = 0; i < filesCount; i++)
        {
            auto fileName = files[i].path;
            auto tex = texture::fromFile(fileName);
            tex->setFullName(getFullName(_texturesPath, fileName));
            _texturesRepository->addResource(tex);
        }
    }

    void resourcesLoader::loadMaterials()
    {
        _materialsRepository = new resourcesRepository();

        loadMaterials(_materialsPath);
    }

    void resourcesLoader::loadMaterials(std::string directory)
    {
        auto subDirs = path::getDirectories(directory);
        auto subDirsCount = subDirs.size();

        for (auto i = 0; i < subDirsCount; i++)
            loadMaterials(subDirs[i].path);

        auto files = path::getFiles(directory);

        auto filesCount = files.size();

        for (auto i = 0; i < filesCount; i++)
            loadMaterial(files[i].path);
    }

    void resourcesLoader::loadMaterial(std::string fileName)
    {
        auto mat = material::fromFile(fileName);

        auto diffuseTexture = _texturesRepository->getResource<texture>(mat->getDiffuseTextureName());

        if (diffuseTexture == nullptr)
            diffuseTexture = _defaultDiffuseTexture;

        mat->setDiffuseTexture(diffuseTexture);

        auto normalTexture = _texturesRepository->getResource<texture>(mat->getNormalTextureName());

        if (normalTexture == nullptr)
            normalTexture = _defaultNormalTexture;

        mat->setNormalTexture(normalTexture);

        auto specularTexture = _texturesRepository->getResource<texture>(mat->getSpecularTextureName());

        if (specularTexture == nullptr)
            specularTexture = _defaultSpecularTexture;

        mat->setSpecularTexture(specularTexture);
        
        auto emissiveTexture = _texturesRepository->getResource<texture>(mat->getEmissiveTextureName());

        if (emissiveTexture == nullptr)
            emissiveTexture = _defaultEmissiveTexture;

        mat->setEmissiveTexture(emissiveTexture);

        mat->setFullName(getFullName(_materialsPath, fileName));

        _materialsRepository->addResource(mat);
    }

    void resourcesLoader::loadModels()
    {
        _modelsRepository = new resourcesRepository();

        loadModels(_modelsPath);
    }

    void resourcesLoader::loadModels(std::string directory)
    {
        auto subDirs = path::getDirectories(directory);
        auto subDirsCount = subDirs.size();

        for (auto i = 0; i < subDirsCount; i++)
            loadModels(subDirs[i].path);

        auto files = path::getFiles(directory);

        auto filesCount = files.size();

        for (auto i = 0; i < filesCount; i++)
            loadModel(files[i].path);
    }

    void resourcesLoader::loadModel(std::string fileName)
    {
        auto model = model::fromFile(fileName);
        
        for (auto mesh : model->getMeshes())
        {
            auto mat = _materialsRepository->getResource<material>(mesh->getMaterialName());

            if (mat == nullptr)
                mat = _defaultMaterial;

            mesh->setMaterial(mat);
        }

        model->setFullName(getFullName(_modelsPath, fileName));

        _modelsRepository->addResource(model);
    }

    void resourcesLoader::load()
    {
        loadTextures();
        loadMaterials();
        loadModels();
    }
}
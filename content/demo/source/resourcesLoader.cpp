#include "resourcesLoader.h"
#include "path.h"
#include "model.h"
#include "fileInfo.h"
#include "directoryInfo.h"
#include "renderingSystem.h"

namespace phi
{
    void resourcesLoader::loadTextures(resourcesRepository* repo)
    {
        auto files = path::getFiles("resources/textures");

        auto filesCount = files.size();

        for (int i = 0; i < filesCount; i++)
        {
            auto tex = texture::fromFile(files[i].path);
            repo->addResource(tex);
        }

        _defaultDiffuseTexture = repo->getResource<texture>("defaultDiffuseTexture.bmp");

        if (_defaultDiffuseTexture == nullptr)
            LOG("defaultDiffuseTexture.bmp texture not found.");

        _defaultNormalTexture = repo->getResource<texture>("defaultNormalTexture.bmp");
        
        if (_defaultNormalTexture == nullptr)
            LOG("defaultNormalTexture.bmp texture not found.");

        _defaultSpecularTexture = repo->getResource<texture>("defaultSpecularTexture.bmp");

        if (_defaultSpecularTexture == nullptr)
            LOG("defaultSpecularTexture.bmp texture not found.");
    }

    void resourcesLoader::loadMaterials(resourcesRepository* repo)
    {
        auto files = path::getFiles("resources/materials");

        auto filesCount = files.size();

        for (int i = 0; i < filesCount; i++)
        {
            auto mat = material::fromFile(files[i].path);

            auto diffuseTexture = repo->getResource<texture>(mat->getDiffuseTextureName());

            if (diffuseTexture == nullptr)
                diffuseTexture = _defaultDiffuseTexture;

            mat->setDiffuseTexture(diffuseTexture);

            auto normalTexture = repo->getResource<texture>(mat->getNormalTextureName());

            if (normalTexture == nullptr)
                normalTexture = _defaultNormalTexture;

            mat->setNormalTexture(normalTexture);

            auto specularTexture = repo->getResource<texture>(mat->getDiffuseTextureName());

            if (specularTexture == nullptr)
                specularTexture = _defaultSpecularTexture;

            mat->setSpecularTexture(specularTexture);

            repo->addResource(mat);
        }

        _defaultMaterial = repo->getResource<material>("default");
        
        if (_defaultMaterial == nullptr)
            LOG("default.material not found.");
    }

    void resourcesLoader::loadModels(resourcesRepository* repo)
    {
        auto dirs = path::getDirectories("resources/models");

        auto dirsCount = dirs.size();

        for (int i = 0; i < dirsCount; i++)
        {
            std::vector<mesh*> meshes;

            auto files = path::getFiles(dirs[i].path);

            for (int i = 0; i < files.size(); i++)
            {
                auto m = mesh::fromMesh(files[i].path);

                if (m == nullptr)
                    continue;

                auto mat = repo->getResource<material>(m->getMaterialName());

                if (mat == nullptr)
                    mat = _defaultMaterial;

                m->setMaterial(mat);

                meshes.push_back(m);
            }

            auto m = new model(dirs[i].name, dirs[i].path, meshes);

            repo->addResource(m);
        }
    }

    resourcesRepository* resourcesLoader::load()
    {
        resourcesRepository* repo = new resourcesRepository();
        
        loadTextures(repo);
        loadMaterials(repo);
        loadModels(repo);

        return repo;
    }
}
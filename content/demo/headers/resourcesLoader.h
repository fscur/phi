#ifndef _PHI_RESOURCES_LOADER_H_
#define _PHI_RESOURCES_LOADER_H_

#include "resourcesRepository.h"
#include "texture.h"
#include "material.h"

namespace phi
{
    class resourcesLoader
    {
    private:
        std::string _texturesPath;
        std::string _materialsPath;
        std::string _modelsPath;

        texture* _defaultDiffuseTexture;
        texture* _defaultNormalTexture;
        texture* _defaultSpecularTexture;
        texture* _defaultEmissiveTexture;
        material* _defaultMaterial;

        resourcesRepository* _texturesRepository;
        resourcesRepository* _materialsRepository;
        resourcesRepository* _modelsRepository;

    private:
        void createDefaultTextures();
        void createDefaultMaterial();
        
        std::string getFullName(std::string relativePath, std::string fileName);

        void loadTextures(std::string directory);
        void loadTextures();

        void loadMaterials();
        void loadMaterials(std::string directory);
        void loadMaterial(std::string fileName);

        void loadModels();
        void loadModels(std::string directory);
        void loadModel(std::string fileName);

    public:
        resourcesLoader();
        material* getDefaultMaterial() const { return _defaultMaterial; }
        resourcesRepository* getTextures() const { return _texturesRepository; }
        resourcesRepository* getMaterias() const { return _materialsRepository; }
        resourcesRepository* getModels() const { return _modelsRepository; }

        void load();
    };
}

#endif
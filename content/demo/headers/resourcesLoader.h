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
        texture* _defaultDiffuseTexture;
        texture* _defaultNormalTexture;
        texture* _defaultSpecularTexture;
        material* _defaultMaterial;

    private:
        void loadTextures(resourcesRepository* repo);
        void loadMaterials(resourcesRepository* repo);
        void loadModels(resourcesRepository* repo);
    public:
        resourcesRepository* load();
    };
}

#endif
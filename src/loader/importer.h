#pragma once
#include <phi.h>
#include "loader.h"
#include <core\resourcesRepository.h>
#include <core\node.h>
#include <rendering\model.h>
#include <rendering\mesh.h>

namespace phi
{
    class LOADER_API importer
    {
    public:
        static material* defaultMaterial;
        static texture* defaultAlbedoTexture;
        static texture* defaultNormalTexture;
        static texture* defaultSpecularTexture;
        static texture* defaultEmissiveTexture;

    private:
        static node* readNode(const rapidjson::Value& node, string currentFolder, resourcesRepository<material>* materialsRepo);
        static guid convertToGuid(const char* bytesGuid);

    public:
        static int importNode(string fileName, resource<node>*& objectResource, resourcesRepository<material>* materialsRepo);
        static int importGeometry(string fileName, phi::geometry*& geometry);
        static int importTexture(string fileName, texture*& texture);
        static int importTexture(string fileName, resource<texture>*& textureResource);
        static int importMaterial(string fileName, resource<material>*& materialResource, resourcesRepository<texture>* texturesRepo);
    };
}
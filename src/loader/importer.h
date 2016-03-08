#pragma once
#include <phi.h>
#include "loader.h"
#include <core\resourcesRepository.h>
#include <core\node.h>
#include <rendering\model.h>
#include <rendering\mesh.h>

namespace phi
{
    class importer
    {
    public:
        LOADER_API static material* defaultMaterial;
        LOADER_API static texture* defaultAlbedoTexture;
        LOADER_API static texture* defaultNormalTexture;
        LOADER_API static texture* defaultSpecularTexture;
        LOADER_API static texture* defaultEmissiveTexture;

    private:
        static node* readNode(const rapidjson::Value& node, string currentFolder, resourcesRepository<material>* materialsRepo);
        static guid convertToGuid(const char* bytesGuid);
    public:
        LOADER_API static int importNode(string fileName, resource<node>*& objectResource, resourcesRepository<material>* materialsRepo);
        LOADER_API static int importer::importGeometry(string fileName, phi::geometry*& geometry);
        LOADER_API static int importer::importTexture(string fileName, texture*& texture);
        LOADER_API static int importer::importTexture(string fileName, resource<texture>*& textureResource);
        LOADER_API static int importer::importMaterial(string fileName, resource<material>*& materialResource, resourcesRepository<texture>* texturesRepo);
    };
}
#pragma once

#include "loader.h"

#include <core\resourcesRepository.h>
#include <core\node.h>
#include <rendering\model.h>
#include <rendering\mesh.h>

#include <rapidjson\document.h>


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
        static node* readNode(const rapidjson::Value& node, std::string currentFolder, resourcesRepository<material>* materialsRepo);
        static GUID convertToGuid(const char* bytesGuid);
    public:
        LOADER_API static int importNode(std::string fileName, resource<node>*& objectResource, resourcesRepository<material>* materialsRepo);
        LOADER_API static int importer::importGeometry(std::string fileName, phi::geometry*& geometry);
        LOADER_API static int importer::importTexture(std::string fileName, texture*& texture);
        LOADER_API static int importer::importTexture(std::string fileName, resource<texture>*& textureResource);
        LOADER_API static int importer::importMaterial(std::string fileName, resource<material>*& materialResource, resourcesRepository<texture>* texturesRepo);
    };
}
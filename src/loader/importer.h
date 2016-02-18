#pragma once

#include "loader.h"

#include <core\resourcesRepository.h>
#include <rendering\model.h>
#include <rendering\mesh.h>

#include <rapidjson\document.h>


namespace phi
{
    class importer
    {
    private:
        static object3D* readNode(const rapidjson::Value& node, std::string currentFolder, resourcesRepository<material>* materialsRepo);
        static GUID convertToGuid(const char* bytesGuid);

    public:
        LOADER_API static int importModel(std::string fileName, model*& resultModel);
        LOADER_API static int importMesh(std::string fileName, std::vector<phi::geometry*>* geometry);
        LOADER_API static int importObject3D(std::string fileName, resource<object3D>*& objectResource, resourcesRepository<material>* materialsRepo);
        LOADER_API static int importer::importGeometry(std::string fileName, phi::geometry*& geometry);
        LOADER_API static int importer::importTexture(std::string fileName, texture*& tex);
        LOADER_API static int importer::importTexture(std::string fileName, resource<texture>*& textureResource);
        LOADER_API static int importer::importMaterial(std::string fileName, resource<material>*& materialResource, resourcesRepository<texture>* texturesRepo);
    };
}
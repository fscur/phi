#ifndef _PHI_IMPORTER_H_
#define _PHI_IMPORTER_H_

#include "loader.h"
#include <phi\core\geometryData.h>
#include <phi\core\object3D.h>
#include <phi\core\resource.h>
#include <phi\rendering\material.h>
#include <phi\rendering\model.h>

#include <rapidjson\document.h>
#include <string>
#include <vector>

namespace phi
{
    class importer
    {
    private:
        static object3D* importer::readNode(const rapidjson::Value& node, std::string currentFolder);

    public:
        LOADER_API static int importModel(std::string fileName, model*& resultModel);
        LOADER_API static int importMesh(std::string fileName, std::vector<geometryData*>* data);
        LOADER_API static int importObject3D(std::string fileName, object3D*& rootNode);
        LOADER_API static int importer::importGeometryData(std::string fileName, geometryData*& data);
        LOADER_API static int importer::importTexture(std::string fileName, texture*& tex);
        LOADER_API static int importer::importTexture(std::string fileName, resource<texture>*& textureResource);
        LOADER_API static int importer::importMaterial(std::string fileName, resource<material>*& materialResource);
    };
}

#endif
#pragma once
#include <phi.h>
#include "loaderApi.h"

#include <core\resourcesRepository.h>
#include <core\node.h>
#include <core\model.h>
#include <core\mesh.h>

#include "assimpImporter.h"

namespace phi
{
    class LOADER_API importer
    {
    private:
        static node* readNode(
            const rapidjson::Value& node,
            const resourcesRepository<material>* materialsRepo,
            const resourcesRepository<geometry>* geometriesRepo);

        static guid convertToGuid(const char* bytesGuid);

    public:
        static image* importImage(const string& fileName);

        static resource<node>* importModel(
            const string& fileName,
            resourcesRepository<material>* materialsRepo,
            resourcesRepository<geometry>* geometriesRepo);

        static resource<image>* loadImage(const string& fileName);
        static resource<geometry>* loadGeometry(const string& fileName);
        static resource<material>* loadMaterial(const string& fileName, const resourcesRepository<image>* texturesRepo);
        static resource<node>* loadNode(
            const string& fileName,
            const resourcesRepository<material>* materialsRepo,
            const resourcesRepository<geometry>* geometriesRepo);

        /*------------------- load .phi file ----------------------------*/

        static vector<node*> loadPhiFile(
            const string& fileName,
            const resourcesRepository<node>* nodeRepository);

        static rapidjson::Document* getJsonDocumentFromPhiFile(
            const string& phiFileContents);

        static std::vector<node*> loadNodes(
            const rapidjson::Document* phiJsonDoc,
            const resourcesRepository<node>* nodeRepository);
        /*------------------------------------------------------------------*/
    };
}
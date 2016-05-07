#pragma once
#include <phi.h>
#include "loaderApi.h"

#include <core\resourcesRepository.h>
#include <core\node.h>

#include <rendering\model.h>
#include <rendering\mesh.h>

namespace phi
{
    class LOADER_API importer
    {
    public:
        static texture* defaultAlbedoTexture;
        static texture* defaultNormalTexture;
        static texture* defaultSpecularTexture;
        static texture* defaultEmissiveTexture;
        static material* defaultMaterial;

    private:
        static node* readNode(const rapidjson::Value& node, string currentFolder, resourcesRepository<material>* materialsRepo, resourcesRepository<geometry>* geometriesRepo);
        static guid convertToGuid(const char* bytesGuid);
		static void loadAssimpScene(
			const aiScene* scene, 
			const aiNode* nd, 
			aiMatrix4x4* transform,
			node* node);

    public:
        static resource<node>* loadNode(string fileName, resourcesRepository<material>* materialsRepo, resourcesRepository<geometry>* geometriesRepo);
        static resource<geometry>* importGeometry(string fileName);
        static texture* importImage(string fileName);
        static resource<texture>* importTexture(string fileName);
        static resource<material>* importMaterial(string fileName, resourcesRepository<texture>* texturesRepo);
		static resource<node>* importModel(string fileName);
    };
}
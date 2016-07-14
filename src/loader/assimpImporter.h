#pragma once
#include <phi.h>
#include <loader\loaderApi.h>
#include <core\node.h>
#include <core\material.h>
#include <core\geometry.h>
#include <core\resourcesRepository.h>

namespace phi
{
    class assimpImporter
    {
    private:
        static void extract3x3(aiMatrix3x3 *m3, aiMatrix4x4 *m4);
        static void loadGeometries(
            const aiScene* scene,
            vector<geometry*>& geometries,
            resourcesRepository<geometry>* geometriesRepo);

        static void loadMaterials(
            const aiScene* scene,
            vector<material*>& materials,
            resourcesRepository<material>* materialsRepo);

        static void loadScene(
            const aiScene* scene,
            const aiNode* nd,
            node* node,
            const vector<material*>& materials,
            const vector<geometry*>& geometries);

    public:
        LOADER_API static resource<node>* import(
            const string& fileName,
            resourcesRepository<material>* materialsRepo,
            resourcesRepository<geometry>* geometriesRepo);
    };
}
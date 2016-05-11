#pragma once
#include <phi.h>
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
		static vector<material*> loadMaterials(const aiScene* scene);
		static void loadScene(
			const aiScene* scene,
			const aiNode* nd,
			aiMatrix4x4* transform,
			node* node,
			vector<material*> materials);

	public:
		static resource<node>* import(
			const string& fileName,
			const resourcesRepository<material>* materialsRepo,
			const resourcesRepository<geometry>* geometriesRepo);
	};
}
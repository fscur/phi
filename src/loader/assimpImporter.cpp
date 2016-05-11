#include <precompiled.h>
#include "assimpImporter.h"
#include <core\random.h>
#include <core\color.h>
#include <core\vertex.h>
#include <core\geometry.h>
#include <core\mesh.h>
#include <io\path.h>
#include "importResourceException.h"

namespace phi
{
	void assimpImporter::extract3x3(aiMatrix3x3 *m3, aiMatrix4x4 *m4)
	{
		m3->a1 = m4->a1; m3->a2 = m4->a2; m3->a3 = m4->a3;
		m3->b1 = m4->b1; m3->b2 = m4->b2; m3->b3 = m4->b3;
		m3->c1 = m4->c1; m3->c2 = m4->c2; m3->c3 = m4->c3;
	}

	vector<material*> assimpImporter::loadMaterials(const aiScene* scene)
	{
		vector<material*> materials;

		for (uint i = 0; i < scene->mNumMaterials; ++i)
		{
			aiMaterial* assimpMaterial = scene->mMaterials[i];
			aiString assimpMaterialName;

			if (assimpMaterial->Get(AI_MATKEY_NAME, assimpMaterialName) == AI_SUCCESS)
			{
				string materialName = string(assimpMaterialName.C_Str());

				auto hue = i <= 8 ? static_cast<float>(i) / 8.0f : random::next();

				auto albedoColor = color::fromHSL(hue, 0.83333f, 0.6667f);

				/*auto material = new phi::material(
					defaultAlbedoTexture,
					defaultNormalTexture,
					defaultSpecularTexture,
					defaultEmissiveTexture,
					vec3(albedoColor.r, albedoColor.g, albedoColor.b));*/

				//materials.push_back(new material());
			}
		}

		return materials;
	}

	void assimpImporter::loadScene(
		const aiScene* scene,
		const aiNode* nd,
		aiMatrix4x4* transform,
		node* node,
		vector<material*> materials)
	{
		aiMatrix4x4 prev = *transform;
		aiMultiplyMatrix4(transform, &nd->mTransformation);

		for (uint n = 0u; n < nd->mNumMeshes; ++n)
		{
			const aiMesh* assimpMesh = scene->mMeshes[nd->mMeshes[n]];
			auto vertices = vector<vertex>();
			auto indices = vector<uint>();

			for (uint i = 0u; i < assimpMesh->mNumVertices; ++i)
			{
				auto position = vec3(0.0f);
				auto texCoord = vec2(0.0f);
				auto normal = vec3(0.0f);

				auto assimpPosition = assimpMesh->mVertices[i];
				aiTransformVecByMatrix4(&assimpPosition, transform);
				position = vec3(assimpPosition.x, assimpPosition.y, assimpPosition.z);

				if (assimpMesh->HasTextureCoords(0))
				{
					const auto assimpTexCoord = assimpMesh->mTextureCoords[0][i];
					texCoord = vec2(assimpTexCoord.x, assimpTexCoord.y);
				}

				if (assimpMesh->HasNormals())
				{
					auto assimpNormal = assimpMesh->mNormals[i];
					aiMatrix3x3 rotation;
					extract3x3(&rotation, transform);
					aiTransformVecByMatrix3(&assimpNormal, &rotation);
					normal = vec3(assimpNormal.x, assimpNormal.y, assimpNormal.z);
				}

				vertices.push_back(vertex(position, texCoord, normal));
			}

			for (uint i = 0u; i < assimpMesh->mNumFaces; ++i)
			{
				const auto assimpFace = assimpMesh->mFaces[i];

				for (uint j = 0u; j < assimpFace.mNumIndices; ++j)
					indices.push_back(assimpFace.mIndices[j]);
			}

			auto geometry = geometry::create(vertices, indices);
			auto meshName = string(assimpMesh->mName.C_Str());
			auto mesh = new phi::mesh(meshName, geometry, materials[assimpMesh->mMaterialIndex]);

			auto meshNode = new phi::node(meshName);
			meshNode->addComponent(mesh);
			node->addChild(meshNode);
		}

		for (uint n = 0u; n < nd->mNumChildren; ++n)
		{
			auto childAssimpNode = nd->mChildren[n];
			auto nodeName = string(childAssimpNode->mName.C_Str());
			auto childNode = new phi::node(nodeName);
			node->addChild(childNode);
			loadScene(scene, childAssimpNode, transform, childNode, materials);
		}

		*transform = prev;
	}

	resource<node>* assimpImporter::import(
		const string& fileName,
		const resourcesRepository<material>* materialsRepo,
		const resourcesRepository<geometry>* geometriesRepo)
	{
#ifdef _DEBUG
		auto stream = aiGetPredefinedLogStream(aiDefaultLogStream_STDOUT, NULL);
		aiAttachLogStream(&stream);
#endif
		const aiScene* assimpScene;
		vector<material*> materials;
		auto flags =
			aiProcess_JoinIdenticalVertices |
			aiProcess_Triangulate |
			aiProcess_GenNormals |
			aiProcess_GenUVCoords |
			aiProcess_ValidateDataStructure |
			aiProcess_ImproveCacheLocality |
			aiProcess_FixInfacingNormals;

		assimpScene = aiImportFile(fileName.c_str(), flags);
		materials = loadMaterials(assimpScene);

		if (assimpScene)
		{
			aiMatrix4x4 transform;
			aiIdentityMatrix4(&transform);

			auto modelName = phi::path::getFileNameWithoutExtension(fileName);
			auto modelNode = new node(modelName);
			loadScene(assimpScene, assimpScene->mRootNode, &transform, modelNode, materials);
			modelNode->optimize();
			return new resource<node>(guidGenerator::newGuid(), modelName, modelNode);
		}

		throw importResourceException("[AssimpImporter] Failed to load model: ", fileName);
	}
}
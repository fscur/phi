#include <precompiled.h>
#include "importer.h"

#include <core\color.h>
#include <core\base64.h>
#include <core\random.h>

#include <io\path.h>

#include "importResourceException.h"
#include <diagnostics\stopwatch.h>

namespace phi
{
    using rapidjson::FileReadStream;
    using rapidjson::Document;
    using rapidjson::Value;

    node* importer::readNode(
		const rapidjson::Value& jsonNode, 
		const string& currentFolder, 
		const resourcesRepository<material>* materialsRepo, 
		const resourcesRepository<geometry>* geometriesRepo)
    {
        auto objectNode = new node();

        const rapidjson::Value& components = jsonNode["Components"];
        auto componentsCount = components.Size();
        for (rapidjson::SizeType i = 0; i < componentsCount; i++)
        {
            auto type = components[i]["Type"].GetInt();
            component* component = nullptr;

            switch (type)
            {
            case 0:
            {
                component = new phi::model(components[i]["Name"].GetString());
                break;
            }
            case 1:
            {
                auto geometryGuid = convertToGuid(components[i]["GeometryResourceGuid"].GetString());
                auto geometry = geometriesRepo->getResource(geometryGuid)->getObject();

                auto materialGuid = convertToGuid(components[i]["MaterialResourceGuid"].GetString());
                auto matRes = materialsRepo->getResource(materialGuid);

                    material* mat = nullptr;

                    if (matRes != nullptr)
                        mat = matRes->getObject();

                component = new phi::mesh(components[i]["Name"].GetString(), geometry, mat);
                break;
            }
            }
            objectNode->addComponent(component);
        }

        const rapidjson::Value& children = jsonNode["Children"];
        auto childrenCount = children.Size();
        for (rapidjson::SizeType i = 0; i < childrenCount; i++)
        {
            auto child = readNode(children[i], currentFolder, materialsRepo, geometriesRepo);
            objectNode->addChild(child);
        }

        return objectNode;
    }

    guid importer::convertToGuid(const char* bytesGuid)
    {
        auto guidBytes = base64::decode(bytesGuid);
        return guidBytes.data();
    }

    image* importer::importImage(const string& fileName)
    {
        auto fileNameChar = fileName.c_str();
        auto imageFormat = FreeImage_GetFileType(fileNameChar, 0);

        if (imageFormat == FIF_UNKNOWN)
            imageFormat = FreeImage_GetFIFFromFilename(fileNameChar);

        if (imageFormat == FIF_UNKNOWN)
            throw importResourceException("Format not identified importing texture", fileName);

        if (!FreeImage_FIFSupportsReading(imageFormat))
            throw importResourceException("Image format not supported ", fileName);

        auto imagePointer = FreeImage_Load(imageFormat, fileNameChar);
        if (!imagePointer)
            throw importResourceException("Failed loading image", fileName);

        auto dataPtr = FreeImage_GetBits(imagePointer);
        auto width = FreeImage_GetWidth(imagePointer);
        auto height = FreeImage_GetHeight(imagePointer);

        if (dataPtr == nullptr || width == 0 || height == 0)
            throw importResourceException("Failed loading image data of", fileName);

        //auto format = GL_BGRA;
		auto format = imageDataFormat::bgra;
        auto bpp = FreeImage_GetBPP(imagePointer);
        auto redMask = FreeImage_GetRedMask(imagePointer);

        switch (bpp)
        {
        case 24:
            if (redMask == 255)
                format = imageDataFormat::rgb;
            else
                format = imageDataFormat::bgr;
            break;
        case 32:
            if (redMask == 255)
                format = imageDataFormat::rgba;
            else
                format = imageDataFormat::bgra;
            break;
        }

        auto bytesPerPixel = bpp / 8;
        auto totalBytes = width * height * bytesPerPixel;
        auto data = new byte[totalBytes];
        memcpy(data, dataPtr, totalBytes);

        FreeImage_Unload(imagePointer);

        return new image(
            width,
            height,
            format,
			imageDataType::ubyte_dataType,
            data);
    }
	
	resource<node>* importer::importModel(
		const string& fileName,
		const resourcesRepository<material>* materialsRepo,
		const resourcesRepository<geometry>* geometriesRepo)
	{
		return assimpImporter::import(
			fileName,
			materialsRepo,
			geometriesRepo);
	}

    resource<image>* importer::loadImage(const string& fileName)
    {
#ifdef _WIN32
        FILE* file;
        fopen_s(&file, fileName.c_str(), "rb"); // non-Windows use "r"
        char readBuffer[65536];

        FileReadStream fileStream(file, readBuffer, sizeof(readBuffer));
        Document document;
        document.ParseStream(fileStream);

        fclose(file);

        auto guid = convertToGuid(document["Guid"].GetString());
        auto imageName = document["ImageFileName"].GetString();

        auto imageFileName = path::getDirectoryFullName(fileName) + "\\" + imageName;
        auto textureName = path::getFileNameWithoutExtension(fileName);
        auto tex = importImage(imageFileName);

        return new resource<image>(guid, textureName, tex);
#else
        throw importResourceException("Import texture was not implemented in other platforms than Win32", fileName);
#endif
    }

    resource<geometry>* importer::loadGeometry(const string& fileName)
    {
#ifdef _WIN32
        std::ifstream file;
        file.open(fileName.c_str(), std::ios::in | std::ios::binary);

        if (!file.is_open())
        {
            throw importResourceException("File coult not be loaded.", fileName);
        }

        auto geometryGuidBytes = new uint8_t[16];
        file.read(reinterpret_cast<char*>(geometryGuidBytes), 16);

        int verticesCount = -1;
        file.read(reinterpret_cast<char*>(&verticesCount), sizeof(int));

        auto positionsBuffer = new float[verticesCount * 3];
        auto texCoordsBuffer = new float[verticesCount * 2];
        auto normalsBuffer = new float[verticesCount * 3];

        auto positionsSize = verticesCount * 3 * sizeof(float);
        auto texCoordsSize = verticesCount * 2 * sizeof(float);
        auto normalsSize = verticesCount * 3 * sizeof(float);

        file.read(reinterpret_cast<char*>(positionsBuffer), positionsSize);
        file.read(reinterpret_cast<char*>(texCoordsBuffer), texCoordsSize);
        file.read(reinterpret_cast<char*>(normalsBuffer), normalsSize);

        int indicesCount = -1;
        file.read(reinterpret_cast<char*>(&indicesCount), sizeof(int));

        auto indicesBuffer = new uint[indicesCount];
        file.read(reinterpret_cast<char*>(indicesBuffer), indicesCount * sizeof(int));

        file.close();

        auto name = path::getFileNameWithoutExtension(fileName);
        auto geometryGuid = guid(geometryGuidBytes);
        safeDelete(geometryGuidBytes);

        auto geom = geometry::create(verticesCount, positionsBuffer, texCoordsBuffer, normalsBuffer, indicesCount, indicesBuffer);
        safeDeleteArray(positionsBuffer);
        safeDeleteArray(texCoordsBuffer);
        safeDeleteArray(normalsBuffer);
        safeDeleteArray(indicesBuffer);

        return new resource<geometry>(geometryGuid, name, geom);
#else
        throw importResourceException("importGeometry was not implemented ins other platforms than WIN32");
#endif
    }

    resource<material>* importer::loadMaterial(const string& fileName, const resourcesRepository<image>* texturesRepo)
    {
#ifdef _WIN32
        FILE* file;
        fopen_s(&file, fileName.c_str(), "rb"); // non-Windows use "r"

        char readBuffer[65536];
        FileReadStream fileStream(file, readBuffer, sizeof(readBuffer));
        Document document;
        document.ParseStream(fileStream);

        fclose(file);

		//TODO: change texture to image into the converter
        auto guid = convertToGuid(document["Guid"].GetString());
        auto albedoImageGuid = convertToGuid(document["AlbedoTextureGuid"].GetString());
        auto normalImageGuid = convertToGuid(document["NormalTextureGuid"].GetString());
        auto specularImageGuid = convertToGuid(document["SpecularTextureGuid"].GetString());
        auto emissiveImageGuid = convertToGuid(document["EmissiveTextureGuid"].GetString());
        auto albedoImageResource = texturesRepo->getResource(albedoImageGuid);
        auto normalImageResource = texturesRepo->getResource(normalImageGuid);
        auto specularImageResource = texturesRepo->getResource(specularImageGuid);
        auto emissiveImageResource = texturesRepo->getResource(emissiveImageGuid);

        const Value& albedoColorNode = document["AlbedoColor"];
        const Value& specularColorNode = document["SpecularColor"];
        const Value& emissiveColorNode = document["EmissiveColor"];
        auto albedoColor = vec3((float)albedoColorNode[0].GetDouble(), (float)albedoColorNode[1].GetDouble(), (float)albedoColorNode[2].GetDouble());
        auto specularColor = vec3((float)specularColorNode[0].GetDouble(), (float)specularColorNode[1].GetDouble(), (float)specularColorNode[2].GetDouble());
        auto emissiveColor = vec3((float)emissiveColorNode[0].GetDouble(), (float)emissiveColorNode[1].GetDouble(), (float)emissiveColorNode[2].GetDouble());

        auto shininess = static_cast<float>(document["Shininess"].GetDouble());
        auto reflectivity = static_cast<float>(document["Reflectivity"].GetDouble());
        auto emission = static_cast<float>(document["Emission"].GetDouble());
        auto opacity = static_cast<float>(document["Opacity"].GetDouble());

		image* albedoImage = nullptr;
		if (albedoImageResource)
			albedoImage = albedoImageResource->getObject();

		image* normalImage = nullptr;
		if (normalImageResource)
			normalImage = normalImageResource->getObject();

		image* specularImage = nullptr;
		if (specularImageResource)
			specularImage = specularImageResource->getObject();

		image* emissiveImage = nullptr;
		if (emissiveImageResource)
			emissiveImage = emissiveImageResource->getObject();

        auto materialName = path::getFileNameWithoutExtension(fileName);
        auto mat = new material(
            albedoImage,
            normalImage,
            specularImage,
            emissiveImage,
            albedoColor,
            specularColor,
            emissiveColor,
            shininess,
            reflectivity,
            emission,
            opacity);

        return new resource<material>(guid, materialName, mat);
#else
        throw importResourceException("Import material was not implemented in other platforms than Win32", fileName);
#endif
    }

	void get_bounding_box_for_node(
		const aiScene* scene,
		const aiNode* nd,
		aiVector3D* min,
		aiVector3D* max,
		aiMatrix4x4* transform)
	{
		unsigned int n = 0, t;

		aiMatrix4x4 prev = *transform;

		aiMultiplyMatrix4(transform, &nd->mTransformation);

		for (; n < nd->mNumMeshes; ++n)
		{
			const struct aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];

			for (t = 0; t < mesh->mNumVertices; ++t)
			{
				aiVector3D tmp = mesh->mVertices[t];
				aiTransformVecByMatrix4(&tmp, transform);

				min->x = std::min(min->x, tmp.x);
				min->y = std::min(min->y, tmp.y);
				min->z = std::min(min->z, tmp.z);

				max->x = std::max(max->x, tmp.x);
				max->y = std::max(max->y, tmp.y);
				max->z = std::max(max->z, tmp.z);
			}
		}

		for (n = 0; n < nd->mNumChildren; ++n) 
		{
			get_bounding_box_for_node(scene, nd->mChildren[n], min, max, transform);
		}

		*transform = prev;
	}

	void extract3x3(aiMatrix3x3 *m3, aiMatrix4x4 *m4)
	{
		m3->a1 = m4->a1; m3->a2 = m4->a2; m3->a3 = m4->a3;
		m3->b1 = m4->b1; m3->b2 = m4->b2; m3->b3 = m4->b3;
		m3->c1 = m4->c1; m3->c2 = m4->c2; m3->c3 = m4->c3;
	}

	void importer::loadAssimpScene(
		const aiScene* scene, 
		const aiNode* nd, 
		aiMatrix4x4* transform,
		node* node)
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

				if (assimpMesh->HasPositions())
				{
					auto assimpPosition = assimpMesh->mVertices[i];
					aiTransformVecByMatrix4(&assimpPosition, transform);
					position = vec3(assimpPosition.x, assimpPosition.y, assimpPosition.z);
				}

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
			auto mesh = new phi::mesh(meshName, geometry, importer::defaultMaterial);

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
			loadAssimpScene(scene, childAssimpNode, transform, childNode);
		}

		*transform = prev;
	}

	resource<node>* importer::importModel(string fileName)
	{
		const aiScene* assimpScene;

		phi::stopwatch::measure([&]
		{
			auto stream = aiGetPredefinedLogStream(aiDefaultLogStream_STDOUT, NULL);
			aiAttachLogStream(&stream);

			auto flags =
				aiProcess_JoinIdenticalVertices |
				aiProcess_Triangulate |
				aiProcess_GenNormals |
				aiProcess_GenUVCoords |
				//aiProcess_OptimizeGraph |
				//aiProcess_ValidateDataStructure |
				//aiProcess_ImproveCacheLocality |
				aiProcess_FixInfacingNormals;

			assimpScene = aiImportFile(fileName.c_str(), flags);

		}, "assimpImportFile");

		if (assimpScene)
		{
			aiMatrix4x4 transform;
			aiIdentityMatrix4(&transform);

			aiVector3D min = aiVector3D(1e10f);
			aiVector3D max = aiVector3D(-1e10f);

			auto rootNode = new node("rootNode");

			phi::stopwatch::measure([&]
			{
				loadAssimpScene(assimpScene, assimpScene->mRootNode, &transform, rootNode);
			}, "loadAssimp");

			rootNode->optimize();

			aiVector3D center;
			center.x = (min.x + max.x) * 0.5f;
			center.y = (min.y + max.y) * 0.5f;
			center.z = (min.z + max.z) * 0.5f;

			guidGenerator::newGuid();

			auto name = string(assimpScene->mRootNode->mName.C_Str());

			return new resource<node>(guidGenerator::newGuid(), name, rootNode);
		}

		throw importResourceException("File to load model: ", fileName);
	}
}
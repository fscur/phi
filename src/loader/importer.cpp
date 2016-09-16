#include <precompiled.h>
#include "importer.h"

#include <animation/animator.h>

#include <core/color.h>
#include <core/base64.h>
#include <core/random.h>
#include <core/boxCollider.h>

#include <io/path.h>
#include <io/fileReader.h>
#include <io/rapidjsonHelper.h>

#include <common/mouseInteractionComponent.h>

#include "importResourceException.h"

namespace phi
{
    using rapidjson::FileReadStream;
    using rapidjson::Document;
    using rapidjson::Value;

    node* importer::readNode(
        const rapidjson::Value& jsonNode,
        const resourcesRepository<material>* materialsRepo,
        const resourcesRepository<geometry>* geometriesRepo)
    {
        string nodeName;

        if (jsonNode.HasMember("Name"))
        {
            const rapidjson::Value& name = jsonNode["Name"];
            nodeName = name.GetString();
        }

        auto objectNode = new node(nodeName);

        const rapidjson::Value& components = jsonNode["Components"];
        auto componentsCount = components.Size();
        for (rapidjson::SizeType i = 0; i < componentsCount; i++)
        {
            auto type = components[i]["Type"].GetInt();

            switch (type)
            {
            case 0:
            {
                objectNode->addComponent(new phi::model());
                break;
            }
            case 1:
            {
                auto geometryGuid = convertToGuid(components[i]["GeometryResourceGuid"].GetString());
                auto geometry = geometriesRepo->getResource(geometryGuid)->getOriginalObject();

                material* material = material::defaultMaterial;

                auto materialGuid = convertToGuid(components[i]["MaterialResourceGuid"].GetString());
                auto materialResource = materialsRepo->getResource(materialGuid);

                if (materialResource != nullptr)
                    material = materialResource->getOriginalObject();

                objectNode->addComponent(new phi::mesh(geometry, material));

                auto aabb = geometry->aabb;
                objectNode->addComponent(new phi::boxCollider(aabb->center, vec3(aabb->width, aabb->height, aabb->depth)));
                objectNode->addComponent(new phi::mouseInteractionComponent());
                objectNode->addComponent(new phi::animator());
                objectNode->setLocalObb(new obb(aabb->center, vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec3(aabb->halfWidth, aabb->halfHeight, aabb->halfDepth)));

                break;
            }
            }
        }

        const rapidjson::Value& children = jsonNode["Children"];
        auto childrenCount = children.Size();
        for (rapidjson::SizeType i = 0; i < childrenCount; i++)
        {
            auto child = readNode(children[i], materialsRepo, geometriesRepo);
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
        resourcesRepository<material>* materialsRepo,
        resourcesRepository<geometry>* geometriesRepo)
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
            albedoImage = albedoImageResource->getOriginalObject();
        else
            albedoImage = image::defaultAlbedoImage;

        image* normalImage = nullptr;
        if (normalImageResource)
            normalImage = normalImageResource->getOriginalObject();
        else
            normalImage = image::defaultNormalImage;

        image* specularImage = nullptr;
        if (specularImageResource)
            specularImage = specularImageResource->getOriginalObject();
        else
            specularImage = image::defaultSpecularImage;

        image* emissiveImage = nullptr;
        if (emissiveImageResource)
            emissiveImage = emissiveImageResource->getOriginalObject();
        else
            emissiveImage = image::defaultEmissiveImage;

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

    resource<node>* importer::loadNode(
        const string& fileName,
        const resourcesRepository<material>* materialsRepo,
        const resourcesRepository<geometry>* geometriesRepo)
    {
        //TODO:: create load file fuction in the io API
#ifdef _WIN32 
        FILE* file;
        fopen_s(&file, fileName.c_str(), "rb"); // non-Windows use "r"

        char fileBuffer[65536];
        FileReadStream fileStream(file, fileBuffer, sizeof(fileBuffer));
        Document document;
        document.ParseStream(fileStream);

        fclose(file);

        auto nodeName = path::getFileNameWithoutExtension(fileName);
        auto rootNode = readNode(document["Node"], materialsRepo, geometriesRepo);

        auto guid = convertToGuid(document["Guid"].GetString());

        aabb* rootAabb = nullptr;
        rootNode->traverse<mesh>([&rootAabb](mesh* mesh)
        {
            if (rootAabb)
                rootAabb = new aabb(aabb::add(*rootAabb, *mesh->getGeometry()->aabb));
            else
                rootAabb = mesh->getGeometry()->aabb;
        });

        // TODO: do not add this collider after the nodes have obb information
        auto rootBoxCollider = new phi::boxCollider(rootAabb->center, vec3(rootAabb->width, rootAabb->height, rootAabb->depth));
        rootBoxCollider->disable();
        rootNode->addComponent(rootBoxCollider);
        rootNode->addComponent(new phi::mouseInteractionComponent());

        auto r = new resource<node>(guid, nodeName, rootNode);
        rootNode->resource = r;

        return r;
#else
        throw importResourceException("importNode was not implemented in other platforms than WIN32", fileName);
#endif
    }

    vector<node*> importer::loadPhiFile(
        const string& fileName,
        const resourcesRepository<node>* nodeRepository)
    {
        const string fileContents = fileReader::readFile(fileName);
        Document* phiJsonDoc = getJsonDocumentFromPhiFile(fileContents);
        return loadNodes(phiJsonDoc, nodeRepository);
    }

    Document* importer::getJsonDocumentFromPhiFile(const string& phiFileContents)
    {
        Document* doc = new Document();
        doc->Parse(phiFileContents.c_str());
        return doc;
    }

    std::vector<node*> importer::loadNodes(
        const Document* phiJsonDoc,
        const resourcesRepository<node>* nodeRepository)
    {
        std::vector<node*> loadedNodes;
        if (phiJsonDoc->IsObject() && phiJsonDoc->HasMember("scene") && phiJsonDoc->HasMember("nodes"))
        {
            auto& nodes = (*phiJsonDoc)["nodes"];
            if (nodes.IsObject())
            {
                for (rapidjson::Value::ConstMemberIterator nodeIt = nodes.MemberBegin(); nodeIt != nodes.MemberEnd(); ++nodeIt)
                {
                    auto nodeGuidStr = nodeIt->value.FindMember("guid")->value.GetString();
                    auto nodeGuid = convertToGuid(nodeGuidStr);
                    auto translation = rapidjsonHelper::iteratorToVec3(nodeIt->value.FindMember("translation"));
                    auto scale = rapidjsonHelper::iteratorToVec3(nodeIt->value.FindMember("scale"));
                    auto rotation = rapidjsonHelper::iteratorToQuat(nodeIt->value.FindMember("rotation"));

                    auto selectedNode = nodeRepository->getResource(nodeGuid)->getClonedObject();
                    if (selectedNode)
                    {
                        selectedNode->getTransform()->setLocalPosition(translation);
                        selectedNode->getTransform()->setLocalOrientation(rotation);
                        selectedNode->getTransform()->setLocalSize(scale);
                        loadedNodes.push_back(selectedNode);
                    }
                }
            }
        }

        return loadedNodes;
    }
}
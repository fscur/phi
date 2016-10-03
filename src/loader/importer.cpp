#include <precompiled.h>
#include "importer.h"

#include <animation/animator.h>

#include <core/color.h>
#include <core/base64.h>
#include <core/random.h>
#include <core/boxCollider.h>
#include <core/transform.h>
#include <core/model.h>
#include <core/modelNode.h>
#include <common/mouseInteractionComponent.h>

#include <io/path.h>
#include <io/fileReader.h>
#include <io/rapidjsonHelper.h>

#include "importResourceException.h"

namespace phi
{
    using namespace rapidjson;

    node* importer::readNode(
        const Value& jsonNode,
        const entityRepository<material>* materialsRepo,
        const entityRepository<geometry>* geometriesRepo)
    {
        string nodeName;

        if (jsonNode.HasMember("Name"))
        {
            const Value& name = jsonNode["Name"];
            nodeName = name.GetString();
        }

        auto objectNode = new node(nodeName);

        const Value& components = jsonNode["Components"];
        auto componentsCount = components.Size();
        for (SizeType i = 0; i < componentsCount; i++)
        {
            auto type = components[i]["Type"].GetInt();

            switch (type)
            {
                //case 0:
                //    objectNode->addComponent(new phi::model());
                //    break;
            case 1:
                auto guid = convertToGuid(components[i]["GeometryResourceGuid"].GetString());
                auto geometry = geometriesRepo->getEntity(guid);

                auto materialGuid = convertToGuid(components[i]["MaterialResourceGuid"].GetString());
                auto material = materialsRepo->getEntity(materialGuid);

                if (material == nullptr)
                    material = material::defaultMaterial;

                objectNode->addComponent(new phi::mesh(geometry, material));

                auto aabb = geometry->aabb;
                objectNode->addComponent(new phi::boxCollider(aabb->center, vec3(aabb->width, aabb->height, aabb->depth)));
                objectNode->addComponent(new phi::animator());
                objectNode->setLocalObb(new obb(aabb->center, vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec3(aabb->halfWidth, aabb->halfHeight, aabb->halfDepth)));

                break;
            }
        }

        const Value& children = jsonNode["Children"];
        auto childrenCount = children.Size();
        for (SizeType i = 0; i < childrenCount; i++)
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

    image* importer::importImage(const string& fileName, guid guid)
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

        //TODO: does all images need a guid?
        return new image(
            guid,
            width,
            height,
            format,
            imageDataType::ubyte_dataType,
            data);
    }

    model* importer::importModel(
        const string& fileName,
        entityRepository<material>* materialsRepo,
        entityRepository<geometry>* geometriesRepo)
    {
        return assimpImporter::import(
            fileName,
            materialsRepo,
            geometriesRepo);
    }

    image* importer::loadImage(const string& fileName)
    {
        auto contents = fileReader::readFile(fileName);
        Document document;
        document.Parse(contents.c_str());

        auto guid = convertToGuid(document["Guid"].GetString());
        auto imageName = document["ImageFileName"].GetString();

        auto imageFileName = path::getDirectoryFullName(fileName) + "\\" + imageName;
        auto textureName = path::getFileNameWithoutExtension(fileName);

        return importImage(imageFileName, guid);
    }

    geometry* importer::loadGeometry(const string& fileName)
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
        auto guid = phi::guid(geometryGuidBytes);
        safeDelete(geometryGuidBytes);

        auto geometry = geometry::create(
            guid,
            verticesCount,
            positionsBuffer,
            texCoordsBuffer,
            normalsBuffer,
            indicesCount,
            indicesBuffer);

        safeDeleteArray(positionsBuffer);
        safeDeleteArray(texCoordsBuffer);
        safeDeleteArray(normalsBuffer);
        safeDeleteArray(indicesBuffer);

        return geometry;
#else
        throw importResourceException("importGeometry was not implemented ins other platforms than WIN32");
#endif
    }

    material* importer::loadMaterial(const string& fileName, const entityRepository<image>* texturesRepo)
    {
        auto contents = fileReader::readFile(fileName);

        Document document;
        document.Parse(contents.c_str());
        //TODO: change texture to image into the converter

        auto guid = convertToGuid(document["Guid"].GetString());
        auto albedoImageGuid = convertToGuid(document["AlbedoTextureGuid"].GetString());
        auto normalImageGuid = convertToGuid(document["NormalTextureGuid"].GetString());
        auto specularImageGuid = convertToGuid(document["SpecularTextureGuid"].GetString());
        auto emissiveImageGuid = convertToGuid(document["EmissiveTextureGuid"].GetString());
        auto albedoImage = texturesRepo->getEntity(albedoImageGuid);
        auto normalImage = texturesRepo->getEntity(normalImageGuid);
        auto specularImage = texturesRepo->getEntity(specularImageGuid);
        auto emissiveImage = texturesRepo->getEntity(emissiveImageGuid);

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

        if (!albedoImage)
            albedoImage = image::defaultAlbedoImage;

        if (!normalImage)
            normalImage = image::defaultNormalImage;

        if (!specularImage)
            specularImage = image::defaultSpecularImage;

        if (!emissiveImage)
            emissiveImage = image::defaultEmissiveImage;

        auto materialName = path::getFileNameWithoutExtension(fileName);
        return new material(
            guid,
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
    }

    model* importer::loadModel(
        const string& fileName,
        const entityRepository<material>* materialsRepo,
        const entityRepository<geometry>* geometriesRepo)
    {
        auto contents = fileReader::readFile(fileName);

        Document document;
        document.Parse(contents.c_str());
        auto nodeName = path::getFileNameWithoutExtension(fileName);

        auto rootNode = readNode(document["Node"], materialsRepo, geometriesRepo);
        rootNode->addComponent(new modelNode());

        bool foundAabb = false;
        aabb rootAabb;
        rootNode->traverse<mesh>([&rootAabb, &foundAabb](mesh* mesh)
        {
            rootAabb = aabb::add(rootAabb, *mesh->getGeometry()->aabb);
            foundAabb = true;
        });

        if (foundAabb)
        {
            auto boxCollider = new phi::boxCollider(rootAabb.center, rootAabb.getSize());
            boxCollider->disable();
            rootNode->addComponent(boxCollider);
        }

        rootNode->addComponent(new phi::mouseInteractionComponent());
        // TODO: do not add this collider after the nodes have obb information
        auto rootBoxCollider = new phi::boxCollider(rootAabb.center, vec3(rootAabb.width, rootAabb.height, rootAabb.depth));
        rootBoxCollider->disable();
        rootNode->addComponent(rootBoxCollider);

        auto guid = convertToGuid(document["Guid"].GetString());
        auto model = new phi::model(guid, rootNode);

        return model;
    }

    vector<node*> importer::loadPhiFile(
        const string& fileName,
        const entityRepository<model>* modelsRepository)
    {
        const string fileContents = fileReader::readFile(fileName);
        Document* phiJsonDoc = getJsonDocumentFromPhiFile(fileContents);
        //auto camera = loadCamera(phiJsonDoc);
        return loadNodes(phiJsonDoc, modelsRepository);
    }

    Document* importer::getJsonDocumentFromPhiFile(const string& phiFileContents)
    {
        Document* doc = new Document();
        doc->Parse(phiFileContents.c_str());
        return doc;
    }

    std::vector<node*> importer::loadNodes(
        const Document* phiJsonDoc,
        const entityRepository<model>* modelsRepository)
    {
        std::vector<node*> loadedNodes;
        if (phiJsonDoc->IsObject() && phiJsonDoc->HasMember("scene") && phiJsonDoc->HasMember("nodes"))
        {
            auto& nodes = (*phiJsonDoc)["nodes"];
            if (nodes.IsObject())
            {
                for (Value::ConstMemberIterator nodeIt = nodes.MemberBegin(); nodeIt != nodes.MemberEnd(); ++nodeIt)
                {
                    auto nodeGuidStr = nodeIt->value.FindMember("guid")->value.GetString();
                    auto nodeGuid = convertToGuid(nodeGuidStr);
                    auto translation = rapidjsonHelper::iteratorToVec3(nodeIt->value.FindMember("translation"));
                    auto scale = rapidjsonHelper::iteratorToVec3(nodeIt->value.FindMember("scale"));
                    auto rotation = rapidjsonHelper::iteratorToQuat(nodeIt->value.FindMember("rotation"));

                    auto selectedNode = modelsRepository->getEntity(nodeGuid)->getNode();
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

    camera* importer::loadCamera(const Document* phiJsonDocument)
    {
        auto& cameraJsonValue = (*phiJsonDocument)["camera"];
        auto near = cameraJsonValue["near"].GetDouble();
        auto far = cameraJsonValue["far"].GetDouble();
        auto fov = cameraJsonValue["fov"].GetDouble();

        auto& transformJsonValue = cameraJsonValue["transform"];
        auto translation = rapidjsonHelper::iteratorToVec3(transformJsonValue.FindMember("translation"));
        auto scale = rapidjsonHelper::iteratorToVec3(transformJsonValue.FindMember("scale"));
        auto orientation = rapidjsonHelper::iteratorToQuat(transformJsonValue.FindMember("orientation"));

        auto transform = new phi::transform();
        transform->setLocalSize(scale);
        transform->setLocalOrientation(orientation);
        transform->setLocalPosition(translation);

        return new camera(
            resolution(1920, 1080),
            transform,
            (float)near,
            (float)far,
            (float)fov);
    }
    }
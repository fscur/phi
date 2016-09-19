#include <precompiled.h>
#include "exporter.h"

using namespace rapidjson;

namespace phi
{
    void exporter::exportMesh(std::vector<geometry*>* geometry, char* fileName)
    {
        auto mat = "Material is not used anymore.";
        std::ofstream stream;
        stream.open(fileName, std::ios::out | std::ios::binary);

        auto meshCount = (*geometry).size();
        for (unsigned int i = 0; i < meshCount; i++)
        {
            auto itemData = (*geometry)[i];
            stream.write((char*)mat, 256);

            auto verticesCount = itemData->verticesCount;
            stream.write((char*)&verticesCount, sizeof(unsigned int));

            stream.write((char*)itemData->vboData, itemData->vboSize);

            auto indicesCount = itemData->indicesCount;
            stream.write((char*)&indicesCount, sizeof(unsigned int));
            stream.write((char*)itemData->eboData, itemData->eboSize);
        }

        safeDeleteArray(mat);

        stream.close();
    }

    void exporter::exportScene(node* root, string path)
    {
        Document document(kObjectType);

        Value nodes(kObjectType);

        Value scene(kObjectType);
        Value sceneNodes(kArrayType);

        Document::AllocatorType& allocator = document.GetAllocator();

        auto namelessNodesCount = 0u;

        for (auto& node : *root->getChildren())
        {
            auto resource = node->resource;
            if (!resource)
                continue;

            auto transform = node->getTransform();
            auto translation = transform->getLocalPosition();
            auto rotation = transform->getLocalOrientation();
            auto scale = transform->getLocalSize();

            auto guidString = resource->getGuid().toStringBase64();
            auto size = static_cast<SizeType>(guidString.length());

            auto nodeName = node->getName().empty() ? "node_" + std::to_string(namelessNodesCount++) : node->getName();
            auto nameLength = static_cast<SizeType>(nodeName.length());

            auto guidValue = Value(guidString.c_str(), size, allocator);

            auto translationValue = rapidjsonHelper::getVec3JsonValue(translation, allocator);
            auto scaleValue = rapidjsonHelper::getVec3JsonValue(scale, allocator);
            auto rotationValue = rapidjsonHelper::getQuatJsonValue(rotation, allocator);

            auto nodeKey = Value(nodeName.c_str(), nameLength, allocator);
            auto nodeValue = Value(kObjectType);

            nodeValue.AddMember("guid", guidValue, allocator);
            nodeValue.AddMember("translation", translationValue, allocator);
            nodeValue.AddMember("rotation", rotationValue, allocator);
            nodeValue.AddMember("scale", scaleValue, allocator);

            nodes.AddMember(nodeKey, nodeValue, allocator);

            auto sceneKey = Value(nodeName.c_str(), nameLength, allocator);
            sceneNodes.PushBack(sceneKey, allocator);
        }

        scene.AddMember("nodes", sceneNodes, allocator);

        document.AddMember("nodes", nodes, allocator);
        document.AddMember("scene", scene, allocator);

        writeDocument(document, path);
    }

    void exporter::writeDocument(const Document& document, string& path)
    {
        FILE* fp;
        auto error = fopen_s(&fp, path.c_str(), "wb"); // non-Windows use "w"
        if (error != 0)
        {
            debug("failed");
            __debugbreak();
        }

        char writeBuffer[65536];
        FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));

        Writer<FileWriteStream> writer(os);
        document.Accept(writer);

        fclose(fp);
    }

    Document exporter::getCameraTransformDocument(transform* transform)
    {
        Document document(kObjectType);
        auto& allocator = document.GetAllocator();
        auto& translation = rapidjsonHelper::getVec3JsonValue(transform->getPosition(), allocator);
        auto& scale = rapidjsonHelper::getVec3JsonValue(transform->getSize(), allocator);
        auto& orientation = rapidjsonHelper::getQuatJsonValue(transform->getOrientation(), allocator);

        document.AddMember("translation", translation, allocator);
        document.AddMember("scale", scale, allocator);
        document.AddMember("orientation", orientation, allocator);
        
        return document;
    }

    Document exporter::exportCamera(camera* camera)
    {
        Document doc(kObjectType);
        auto& allocator = doc.GetAllocator();
        auto& transform = getCameraTransformDocument(camera->getTransform());

        doc.AddMember("transform", transform, allocator);
        doc.AddMember("near", camera->getNear(), allocator);
        doc.AddMember("far", camera->getFar(), allocator);
        doc.AddMember("fov", camera->getFov(), allocator);

        return doc;
    }
}
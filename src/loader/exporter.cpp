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
        Value scene(kObjectType);
        Value nodes(kArrayType);

        Document::AllocatorType& allocator = document.GetAllocator();

        for (auto& node : *root->getChildren())
        {
            auto resource = node->resource;
            if (!resource)
                continue;

            auto guidString = resource->getGuid().toStringBase64();
            auto size = static_cast<SizeType>(guidString.length());

            auto value = Value(guidString.c_str(), size, allocator);
            nodes.PushBack(value, allocator);
        }

        scene.AddMember("nodes", nodes, allocator);
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
#pragma once
#include <phi.h>
#include "loaderApi.h"

#include <core/geometry.h>
#include <core/node.h>
#include <rendering/camera.h>

namespace phi
{
    class LOADER_API exporter
    {
    public:
        static void exportMesh(vector<geometry*>* geometry, char* fileName);
        static void exportScene(node* root, string path);
        static rapidjson::Document exportCamera(camera* camera);
    private:
        static void writeDocument(const rapidjson::Document & document, string & path);
        static rapidjson::Document getCameraTransformDocument(transform* transform);
        static rapidjson::Value getVec3JsonValue(
            vec3 vec, 
            rapidjson::Document::AllocatorType& allocator);
        static rapidjson::Value getQuatJsonValue(
            quat quat, 
            rapidjson::Document::AllocatorType& allocator);
    };
}
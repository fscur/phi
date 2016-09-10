#pragma once
#include <phi.h>
#include "loaderApi.h"

#include <core/geometry.h>
#include <core/node.h>

namespace phi
{
    class LOADER_API exporter
    {
    public:
        static void exportMesh(vector<geometry*>* geometry, char* fileName);
        static void exportScene(node* root, string path);

    private:
        static void writeDocument(const rapidjson::Document & document, string & path);
    };
}
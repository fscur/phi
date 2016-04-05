#pragma once
#include <phi.h>
#include "loaderApi.h"
#include <core\geometry.h>

namespace phi
{
    class LOADER_API exporter
    {
    public:
        static void exportMesh(vector<geometry*>* geometry, char* fileName);
    };
}
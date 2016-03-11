#pragma once
#include <phi.h>
#include "loader.h"
#include <core\geometry.h>

namespace phi
{
    class LOADER_API exporter
    {
    public:
        static int exportMesh(vector<geometry*>* geometry, char* fileName);
    };
}
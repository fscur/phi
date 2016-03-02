#pragma once

#include "loader.h"

#include <core\geometry.h>

namespace phi
{
    class exporter
    {
    public:
        LOADER_API static int exportMesh(std::vector<geometry*>* geometry, char* fileName);
    };
}
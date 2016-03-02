#pragma once
#include <precompiled.h>
#include "loader.h"
#include <core\geometry.h>

namespace phi
{
    class exporter
    {
    public:
        LOADER_API static int exportMesh(vector<geometry*>* geometry, char* fileName);
    };
}
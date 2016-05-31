#pragma once
#include <phi.h>
#include <core\mesh.h>

namespace phi
{
    struct renderInstance
    {
        mesh* mesh;
        uint materialId;
        mat4 modelMatrix;
    };
}
#pragma once
#include <phi.h>
#include <rendering\mesh.h>

namespace phi
{
    struct renderInstance
    {
        mesh* mesh;
        uint materialId;
        mat4 modelMatrix;
    };
}
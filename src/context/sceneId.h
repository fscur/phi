#pragma once
#include <phi.h>
#include "contextApi.h"

#include <core\mesh.h>

namespace phi
{
    class sceneId
    {
    private:
        static int _currentId;
        static map<int, mesh*> _meshesIds;
    public:
        CONTEXT_API static void setNextId(mesh* mesh);
        CONTEXT_API static mesh* getMesh(int id);
        CONTEXT_API static void removeMeshId(mesh* mesh);
    };
}
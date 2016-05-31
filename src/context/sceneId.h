#pragma once
#include <phi.h>
#include "scenesApi.h"

#include <core\mesh.h>

namespace phi
{
    class sceneId
    {
    private:
        static int _currentId;
        static map<int, mesh*> _meshesIds;
    public:
        SCENES_API static void setNextId(mesh* mesh);
        SCENES_API static mesh* getMesh(int id);
        SCENES_API static void removeMeshId(mesh* mesh);
    };
}
#pragma once
#include <phi.h>
#include <rendering\mesh.h>

namespace phi
{
    class sceneId
    {
    private:
        static int _currentId;
        static map<int, mesh*> _meshesIds;
    public:
        static void setNextId(mesh* mesh);
        static mesh* getMesh(int id);
        static void removeMeshId(mesh* mesh);
    };
}
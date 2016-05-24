#include <precompiled.h>
#include "sceneId.h"

namespace phi
{
    map<int, mesh*> sceneId::_meshesIds = map<int, mesh*>();
    int sceneId::_currentId = 0;

    void sceneId::setNextId(mesh* mesh)
    {
        ++_currentId;

        mesh->setId(_currentId);
        _meshesIds[_currentId] = mesh;
    }

    mesh* sceneId::getMesh(int id)
    {
        return _meshesIds[id];
    }

    void sceneId::removeMeshId(mesh* mesh)
    {
        _meshesIds.erase(mesh->getId());
    }
}
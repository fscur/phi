#include "phi/rendering/meshRenderer.h"

namespace phi
{
    mesh* meshRenderer::_lastMesh = nullptr;

    void meshRenderer::render(mesh* mesh)
    {
        if (mesh != _lastMesh)
        {
            if (_lastMesh != nullptr)
                _lastMesh->unbind();

            mesh->bind();
            _lastMesh = mesh;
        }

        mesh->render();
    }
}
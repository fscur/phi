#ifndef _PHI_MESH_RENDERER_H_
#define _PHI_MESH_RENDERER_H_

#include "mesh.h"

namespace phi
{
    class meshRenderer
    {
    private:
        static mesh* _lastMesh;
    public:
        RENDERING_API static void render(mesh* mesh);
    };
}

#endif
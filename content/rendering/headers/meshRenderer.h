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
        static void render(mesh* mesh);
    };
}

#endif
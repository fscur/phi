#ifndef _PHI_GEOMETRY_RENDERER_H_
#define _PHI_GEOMETRY_RENDERER_H_

#include "geometry.h"

namespace phi
{
    class geometryRenderer
    {
    private:
        static geometry* _lastGeometry;
    public:
        RENDERING_API static void render(geometry* geometry);
    };
}

#endif
#ifndef _PHI_CONE_H_
#define _PHI_CONE_H_

#include <phi/rendering/mesh.h>

#include "scenes.h"

namespace phi
{
    class cone :
        public mesh
    {
    private:
        cone(geometry* geometry, material* material);

    private:
        static geometry* createConeGeometry(uint sectors);

    public:
        SCENES_API static cone* create(uint sectors, material* material);

    public:
        SCENES_API ~cone();
    };
}

#endif
#ifndef _PHI_CONE_H_
#define _PHI_CONE_H_

#include "scenes.h"

#include <rendering\mesh.h>

namespace phi
{
    class cone :
        public mesh
    {
    private:
        cone(phi::geometry* geometry, phi::material* material);

    private:
        static phi::geometry* createConeGeometry(uint sectors);

    public:
        SCENES_API static cone* create(uint sectors, phi::material* material);

    public:
        SCENES_API ~cone();
    };
}

#endif
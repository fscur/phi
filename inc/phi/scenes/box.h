#ifndef _PHI_BOX_H
#define _PHI_BOX_H

#include "phi/rendering/material.h"
#include "phi/scenes/sceneObject.h"

namespace phi
{
    class box :
        public sceneObject
    {
    private:
        mesh* create(phi::size<float> size);

    public:
        SCENES_API box(phi::size<float> size, phi::material* material);
        SCENES_API ~box(void);
    };
}

#endif
#ifndef _PHI_MODEL_H_
#define _PHI_MODEL_H_

#include <core\object3D.h>

#include "rendering.h"

namespace phi
{
    class model :
        public object3D
    {
    public:
        RENDERING_API model(std::string name) :
            object3D(name, objectType::MODEL)
        {}

        RENDERING_API model(const model& clone) :
            object3D(clone)
        {
        }
    };
}
#endif
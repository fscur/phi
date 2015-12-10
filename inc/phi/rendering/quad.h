#ifndef _PHI_QUAD_H_
#define _PHI_QUAD_H_

#include "rendering.h"

#include <phi/core/geometryData.h>

namespace phi
{
    class quad :
        public geometryData
    {
    public:
        RENDERING_API quad();
        RENDERING_API ~quad();
    };
}

#endif
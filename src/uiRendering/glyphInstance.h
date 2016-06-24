#pragma once
#include <phi.h>
#include "glyphRenderData.h"

namespace phi
{
    struct glyphInstance
    {
        glyphInstance(glyphRenderData renderData, mat4 modelMatrix) :
            renderData(renderData),
            modelMatrix(modelMatrix)
        {
        }

        glyphRenderData renderData;
        mat4 modelMatrix;
    };
}

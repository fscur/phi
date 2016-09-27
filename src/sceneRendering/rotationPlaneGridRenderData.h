#pragma once
#include <core\color.h>
#include <core\rotationPlaneGrid.h>
#include <rendering\texturesManager.h>
#include <core\time.h>

namespace phi
{
    struct rotationPlaneGridRenderData
    {
        color color;
        
        float lineThickness;
        float opacity;
        float visibility;
        float pad1;

        static rotationPlaneGridRenderData from(const rotationPlaneGrid* planeGrid)
        {
            auto data = rotationPlaneGridRenderData();
            data.color = planeGrid->getColor();
            data.lineThickness = planeGrid->getLineThickness();
            data.opacity = planeGrid->getOpacity();
            data.visibility = planeGrid->getVisibilityFactor();

            return data;
        }
    };
}

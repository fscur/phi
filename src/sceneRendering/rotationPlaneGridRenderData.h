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
        float angle;

        static rotationPlaneGridRenderData from(const rotationPlaneGrid* planeGrid)
        {
            auto data = rotationPlaneGridRenderData();
            data.color = planeGrid->getColor();
            data.lineThickness = planeGrid->getLineThickness();
            data.opacity = planeGrid->getOpacity();
            data.visibility = planeGrid->getVisibilityFactor();
            data.angle = planeGrid->getFilledAngle();

            return data;
        }
    };
}

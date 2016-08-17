#pragma once
#include <core\color.h>
#include <ui\planeGrid.h>
#include <rendering\texturesManager.h>
#include <core\time.h>
namespace phi
{
    struct planeGridRenderData
    {
        color color;
        float lineThickness;
        float opacity;
        float pad0;
        float pad1;

        static planeGridRenderData from(const planeGrid* planeGrid)
        {
            auto data = planeGridRenderData();
            data.color = planeGrid->getColor();
            data.lineThickness = planeGrid->getLineThickness();
            data.opacity = planeGrid->getOpacity();
            return data;
        }
    };
}

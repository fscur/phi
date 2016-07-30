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
        float startTime;
        float lineThickness;
        float pad0;
        float pad1;

        static planeGridRenderData from(const planeGrid* planeGrid)
        {
            auto data = planeGridRenderData();
            data.color = planeGrid->getColor();
            data.startTime = static_cast<float>(time::totalSeconds);
            data.lineThickness = planeGrid->getLineThickness();
            return data;
        }
    };
}

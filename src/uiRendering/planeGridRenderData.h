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
        float planeSize;
        float pad0;
        float pad1;

        static planeGridRenderData from(const planeGrid* planeGrid)
        {
            auto data = planeGridRenderData();
            data.color = planeGrid->getColor();
            data.startTime = static_cast<float>(time::totalSeconds);
            data.planeSize = planeGrid->getSize();
            return data;
        }
    };
}

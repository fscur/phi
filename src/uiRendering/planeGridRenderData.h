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
        vec4 clipPlane0;
        vec4 clipPlane1;
        vec4 clipPlane2;
        float lineThickness;
        float opacity;
        float pad0;
        float pad1;

        static vector<vec4> getClippingPlanes(const planeGrid* planeGrid)
        {
            vector<vec4> planes = { vec4(0.0), vec4(0.0), vec4(0.0) };

            auto clippingPlanes = planeGrid->getClippingPlanes();
            auto count = clippingPlanes.size();

            for (size_t i = 0; i < count; i++)
            {
                if (i > 2)
                    break;

                planes[i] = clippingPlanes[i].toVec4();
            }

            return planes;
        }

        static planeGridRenderData from(const planeGrid* planeGrid)
        {
            auto data = planeGridRenderData();
            data.color = planeGrid->getColor();
            data.lineThickness = planeGrid->getLineThickness();
            data.opacity = planeGrid->getOpacity();
            
            auto clippingPlanes = getClippingPlanes(planeGrid);
            
            data.clipPlane0 = clippingPlanes[0];
            data.clipPlane1 = clippingPlanes[1];
            data.clipPlane2 = clippingPlanes[2];

            return data;
        }
    };
}

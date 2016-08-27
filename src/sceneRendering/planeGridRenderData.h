#pragma once
#include <core\color.h>
#include <core\planeGrid.h>
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

        float clipPlane0Opacity;
        float clipPlane1Opacity;
        float clipPlane2Opacity;
        float pad2;

        static planeGridRenderData from(const planeGrid* planeGrid)
        {
            auto data = planeGridRenderData();
            data.color = planeGrid->getColor();
            data.lineThickness = planeGrid->getLineThickness();
            data.opacity = planeGrid->getOpacity();
            
            vector<vec4> planes = { vec4(0.0), vec4(0.0), vec4(0.0) };
            vector<float> opacities = { 0.0f, 0.0f, 0.0f };

            auto clippingPlanes = planeGrid->getClippingPlanes();
            auto clippingPlanesOpacities = planeGrid->getClippingPlanesOpacities();
            auto clippingPlanesDistances = planeGrid->getClippingPlanesDistances();

            auto count = clippingPlanes.size();

            for (size_t i = 0; i < count; i++)
            {
                if (i > 2)
                    break;

                auto clippingPlane = clippingPlanes[i];
                planes[i] = clippingPlane->toVec4(clippingPlanesDistances[clippingPlane]);
                opacities[i] = clippingPlanesOpacities[clippingPlane];
            }
            
            data.clipPlane0 = planes[0];
            data.clipPlane1 = planes[1];
            data.clipPlane2 = planes[2];
            data.clipPlane0Opacity = opacities[0];
            data.clipPlane1Opacity = opacities[1];
            data.clipPlane2Opacity = opacities[2];

            return data;
        }
    };
}

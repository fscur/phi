#pragma once
#include <core\color.h>
#include <ui\planeGrid.h>
#include <rendering\texturesManager.h>

namespace phi
{
    struct planeGridRenderData
    {
        color color;
        int textureUnit;
        int pad0;
        int pad1;
        int pad2;
        float texturePage;
        float pad3;
        float pad4;
        float pad5;

        static planeGridRenderData from(const planeGrid* planeGrid)
        {
            auto texture = texturesManager::getTextureFromImage(planeGrid->getImage(), false);
            textureAddress address;

            if (!texturesManager::contains(texture))
                address = texturesManager::add(texture);
            else
                address = texturesManager::getTextureAddress(texture);

            auto data = planeGridRenderData();
            data.color = color::red;
            data.textureUnit = address.unit;
            data.texturePage = address.page;
            return data;
        }
    };
}

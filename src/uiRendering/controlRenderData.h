#pragma once
#include <core\color.h>
#include <ui\control.h>
#include <rendering\texturesManager.h>

namespace phi
{
    struct controlRenderData
    {
        color backgroundColor;
        int backgroundTextureUnit;
        int pad0;
        int pad1;
        int pad2;
        float backgroundTexturePage;
        float pad3;
        float pad4;
        float pad5;

        static controlRenderData from(const control* control)
        {
            auto texture = texturesManager::getTextureFromImage(control->getBackgroundImage());
            textureAddress address;

            if (!texturesManager::contains(texture))
                address = texturesManager::add(texture);
            else
                address = texturesManager::getTextureAddress(texture);

            auto renderData = controlRenderData();
            renderData.backgroundColor = control->getBackgroundColor();
            renderData.backgroundTextureUnit = address.unit;
            renderData.backgroundTexturePage = address.page;

            return renderData;
        }
    };
}

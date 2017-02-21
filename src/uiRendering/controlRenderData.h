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
        int isGlassy;
        int pad1;
        int pad2;
        float backgroundTexturePage;
        float pad3;
        float pad4;
        float pad5;

        static controlRenderData from(const control* control)
        {
            auto texture = texturesManager::getTextureFromImage(control->getBackgroundImage(), false);
            textureAddress address;

            if (!texturesManager::contains(texture))
                address = texturesManager::addTexture(texture);
            else
                address = texturesManager::getTextureAddress(texture);

            auto renderData = controlRenderData();
            renderData.backgroundColor = control->getColor();
            renderData.backgroundTextureUnit = address.index;
            renderData.backgroundTexturePage = address.page;
            renderData.isGlassy = control->isGlassy();

            return renderData;
        }
    };
}

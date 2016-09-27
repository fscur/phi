#pragma once
#include <core\color.h>
#include <core\skyBox.h>
#include <rendering\texturesManager.h>
#include <core\time.h>

namespace phi
{
    struct skyBoxRenderData
    {
        color color;

        static skyBoxRenderData from(const skyBox* skyBox)
        {
            auto data = skyBoxRenderData();
            data.color = skyBox->getColor();
            return data;
        }
    };
}

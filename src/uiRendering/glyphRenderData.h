#pragma once
#include <phi.h>

#include <core\color.h>

#include <rendering\glyphTextureData.h>

namespace phi
{
    struct glyphRenderData
    {
        glyphRenderData()
        {}

        glyphRenderData(const glyphTextureData* glyphTextureData, float shift, color color) :
            color(color),
            position(glyphTextureData->texturePosition),
            size(glyphTextureData->textureSize),
            texelSize(glyphTextureData->texelSize),
            shift(shift),
            page(glyphTextureData->page),
            unit(glyphTextureData->unit)
        {
        }

        color color;
        vec2 position;
        vec2 size;
        vec2 texelSize;
        float shift;
        float page;
        int unit;
        int parentModelMatrixIndex;
        int pad3;
        int pad4;
    };
}
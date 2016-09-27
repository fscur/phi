#pragma once
#include <phi.h>
#include "glyphInstance.h"
#include "glyphRenderData.h"
#include <ui\text.h>
#include <rendering\fontsManager.h>
#include <core\node.h>

namespace phi
{
    class textInstance
    {
    public:
        textInstance() :
            glyphsRenderData(vector<glyphRenderData>()),
            modelMatrices(vector<mat4>()),
            parentModelMatrixIndex(),
            textChangedEventToken()
        {
        }

    public:
        vector<glyphRenderData> glyphsRenderData;
        vector<mat4> modelMatrices;
        int parentModelMatrixIndex;
        eventToken textChangedEventToken;
    };
}
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
    private:
        vector<glyphRenderData> _glyphsRenderData;
        vector<mat4> _modelMatrices;

    public:
        textInstance() :
            _glyphsRenderData(vector<glyphRenderData>()),
            _modelMatrices(vector<mat4>())
        {
        }

        void add(const mat4& modelMatrix)
        {
            _modelMatrices.push_back(modelMatrix);
        }

        void add(const glyphRenderData& renderData)
        {
            _glyphsRenderData.push_back(renderData);
        }

        vector<mat4> getModelMatrices() const { return _modelMatrices; }
        vector<glyphRenderData> getGlyphsRenderData() const { return _glyphsRenderData; }
    };
}
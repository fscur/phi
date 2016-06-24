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

        vector<mat4> getModelMatrices() const { return _modelMatrices; }
        vector<glyphRenderData> getGlyphsRenderData() const { return _glyphsRenderData; }

    public:
        static textInstance* from(phi::text* text)
        {
            auto textInstance = new phi::textInstance();

            auto font = text->getFont();
            auto textString = text->getText();
            auto position = text->getNode()->getTransform()->getPosition();

            float baseLine = font->getBaseLine();
            float spacing = font->getSpacing();
            float x = position.x + spacing;
            float y = position.y - baseLine + spacing;
            float z = position.z;

            glyph* previousGlyph = nullptr;

            auto glyphTextureData = fontsManager::getGlyph(font, (ulong)textString[0]);
            x -= glyphTextureData->glyph->offsetX;

            auto textLength = textString.length();
            for (auto i = 0; i < textLength; i++)
            {
                glyphTextureData = fontsManager::getGlyph(font, (ulong)textString[i]);
                auto glyph = glyphTextureData->glyph;

                auto kern = font->getKerning(previousGlyph, glyphTextureData->glyph);
                auto w = glyph->width;
                auto h = glyph->height;
                auto x0 = x + glyph->offsetX;
                auto y0 = y - h + glyph->offsetY;

                auto modelMatrix = mat4(
                    w, 0.0f, 0.0f, 0.0f,
                    0.0f, -h, 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    x0, y0 + h, z, 1.0f);

                x += glyph->horiAdvance + kern.x;

                float shift = std::abs(x0 - static_cast<int>(x0));

                auto renderData = glyphRenderData(glyphTextureData, shift, text->getColor());

                textInstance->_glyphsRenderData.push_back(renderData);
                textInstance->_modelMatrices.push_back(modelMatrix);

                previousGlyph = glyph;
            }

            return textInstance;
        }
    };
}
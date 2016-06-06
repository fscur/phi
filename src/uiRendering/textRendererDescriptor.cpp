#include <precompiled.h>
#include "textRenderer.h"

#include <core\node.h>
#include <core\geometry.h>
#include <core\notImplementedException.h>

namespace phi
{
    textRendererDescriptor::textRendererDescriptor(gl* gl) :
        _gl(gl)
    {
        createBuffers();
    }

    textRendererDescriptor::~textRendererDescriptor()
    {
        for (auto pair : _instances)
            safeDelete(pair.second);

        safeDelete(_glyphRenderDataBuffer);
        safeDelete(_modelMatricesBuffer);
    }

    void textRendererDescriptor::createBuffers()
    {
        _vao = new instancedTextVao();

        _glyphRenderDataBuffer = new buffer(bufferTarget::shader);
        _glyphRenderDataBuffer->data(sizeof(glyphRenderData), nullptr, bufferDataUsage::dynamicDraw);
    }

    void textRendererDescriptor::recreateBuffers()
    {
        auto renderData = vector<glyphRenderData>();
        auto modelMatrices = vector<mat4>();

        auto instancesCount = 0u;
        for (auto pair : _instances)
        {
            for (auto instance : pair.second->glyphs)
            {
                ++instancesCount;
                renderData.push_back(instance->renderData);
                modelMatrices.push_back(instance->modelMatrix);
            }
        }

        _glyphRenderDataBuffer->data(sizeof(glyphRenderData) * instancesCount, &renderData[0], bufferDataUsage::dynamicDraw);
        _vao->update(modelMatrices);
    }

    textRendererDescriptor::textInstance* textRendererDescriptor::buildTextInstance(text* text)
    {
        vector<glyphInstance*> glyphInstances;

        auto font = text->getFont();
        auto textString = text->getText();
        auto position = text->getNode()->getTransform()->getPosition();

        float baseLine = font->getBaseLine();
        float spacing = font->getSpacing();
        float x = position.x + spacing;
        float y = position.y - baseLine + spacing;
        float z = position.z;

        glyph* previousGlyph = nullptr;
        auto textLength = textString.length();

        auto glyph = _gl->fontsManager->getGlyph(font, (ulong)textString[0]);
        x -= glyph->offsetX;

        for (auto i = 0; i < textLength; i++)
        {
            glyph = _gl->fontsManager->getGlyph(font, (ulong)textString[i]);
            auto kern = font->getKerning(previousGlyph, glyph);
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

            auto renderData = glyphRenderData();
            renderData.position = glyph->texturePosition;
            renderData.size = glyph->textureSize;
            renderData.shift = shift;
            renderData.unit = glyph->unit;
            renderData.page = glyph->page;
            renderData.texelSize = glyph->texelSize;
            renderData.color = text->getColor();

            auto glyphInstance = new textRendererDescriptor::glyphInstance();
            glyphInstance->modelMatrix = modelMatrix;
            glyphInstance->renderData = renderData;

            glyphInstances.push_back(glyphInstance);

            previousGlyph = glyph;
        }

        auto instance = new textInstance();
        instance->glyphs = glyphInstances;

        return instance;
    }

    void textRendererDescriptor::add(text* text)
    {
        _instances[text] = buildTextInstance(text);
        recreateBuffers();
    }

    void textRendererDescriptor::remove(text* text)
    {
        _instances.erase(text);
        recreateBuffers();
    }

    void textRendererDescriptor::update(text* text)
    {
        _instances[text] = buildTextInstance(text);
        recreateBuffers();
    }
}
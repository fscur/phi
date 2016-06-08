#include <precompiled.h>
#include "textRenderer.h"

#include <core\node.h>
#include <core\geometry.h>
#include <core\notImplementedException.h>

namespace phi
{
    textRendererDescriptor::textRendererDescriptor(gl* gl) :
        _gl(gl),
        _glyphCount(0)
    {
        createBuffers();
    }

    textRendererDescriptor::~textRendererDescriptor()
    {
        for (auto pair : _instances)
            safeDelete(pair.second);

        safeDelete(_modelMatricesBuffer);
        safeDelete(_glyphRenderDataBuffer);
        safeDelete(_modelMatricesBuffer);
    }

    void textRendererDescriptor::createBuffers()
    {
        _glyphRenderDataBuffer = new buffer("GlyphRenderData", bufferTarget::shader);
        _glyphRenderDataBuffer->data(sizeof(glyphRenderData), nullptr, bufferDataUsage::dynamicDraw);

        auto vertices = vector<vertex>
        {
            vertex(vec3(0.0f, 0.0f, +0.0f), vec2(0.0f, 0.0f)),
            vertex(vec3(1.0f, 0.0f, +0.0f), vec2(1.0f, 0.0f)),
            vertex(vec3(1.0f, 1.0f, +0.0f), vec2(1.0f, 1.0f)),
            vertex(vec3(0.0f, 1.0f, +0.0f), vec2(0.0f, 1.0f))
        };
        auto indices = vector<uint>{ 0, 1, 2, 2, 3, 0 };
        auto textQuad = geometry::create(vertices, indices);

        vector<vertexAttrib> vboAttribs;
        vboAttribs.push_back(vertexAttrib(0, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::position)));
        vboAttribs.push_back(vertexAttrib(1, 2, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::texCoord)));

        auto vbo = new vertexBuffer("vbo", vboAttribs);
        vbo->storage(textQuad->vboSize, textQuad->vboData, bufferStorageUsage::dynamic | bufferStorageUsage::write);

        auto ebo = new buffer("ebo", bufferTarget::element);
        ebo->storage(textQuad->eboSize, textQuad->eboData, bufferStorageUsage::dynamic | bufferStorageUsage::write);

        vector<vertexAttrib> modelMatricesAttribs;

        for (uint i = 0; i < 4; ++i)
            modelMatricesAttribs.push_back(vertexAttrib(2 + i, 4, GL_FLOAT, sizeof(mat4), (const void*)(sizeof(GLfloat) * i * 4), 1));

        _modelMatricesBuffer = new vertexBuffer("modelMatrices", modelMatricesAttribs);
        _modelMatricesBuffer->data(sizeof(mat4), nullptr, bufferDataUsage::dynamicDraw);

        _vao = new vertexArrayObject();
        _vao->add(vbo);
        _vao->add(_modelMatricesBuffer);
        _vao->setEbo(ebo);

        _vao->setOnRender([=]
        {
            glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, _glyphCount);
        });
    }

    void textRendererDescriptor::recreateBuffers()
    {
        auto renderData = vector<glyphRenderData>();
        auto modelMatrices = vector<mat4>();

        _glyphCount = 0u;
        for (auto pair : _instances)
        {
            for (auto instance : pair.second->glyphs)
            {
                ++_glyphCount;
                renderData.push_back(instance->renderData);
                modelMatrices.push_back(instance->modelMatrix);
            }
        }

        _modelMatricesBuffer->data(sizeof(mat4) * _glyphCount, &modelMatrices[0], bufferDataUsage::dynamicDraw);
        _glyphRenderDataBuffer->data(sizeof(glyphRenderData) * _glyphCount, &renderData[0], bufferDataUsage::dynamicDraw);
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
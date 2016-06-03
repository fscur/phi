#include <precompiled.h>
#include "textRenderer.h"

#include <core\node.h>
#include <core\geometry.h>
#include <core\notImplementedException.h>

namespace phi
{
    textRenderer::textRenderer(gl* gl) :
        _gl(gl)
    {
        createBuffers();
        _texelSize = _gl->fontsManager->getGlyphAtlasContainer()->getTexelSize();
    }

    textRenderer::~textRenderer()
    {
        for (auto pair : _instances)
            safeDelete(pair.second);

        safeDelete(_glyphIdsBuffer);
        safeDelete(_glyphRenderDataBuffer);
        safeDelete(_modelMatricesBuffer);
    }

    void textRenderer::createBuffers()
    {
        auto vertices = vector<vertex>
        {
            vertex(vec3(0.0f, 0.0f, +0.0f), vec2(0.0f, 0.0f)),
            vertex(vec3(1.0f, 0.0f, +0.0f), vec2(1.0f, 0.0f)),
            vertex(vec3(1.0f, 1.0f, +0.0f), vec2(1.0f, 1.0f)),
            vertex(vec3(0.0f, 1.0f, +0.0f), vec2(0.0f, 1.0f))
        };
        auto indices = vector<uint>{ 0, 1, 2, 2, 3, 0 };
        auto textQuad = geometry::create(vertices, indices);

        glCreateVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

        vector<vertexAttrib> textVboAttribs;
        textVboAttribs.push_back(vertexAttrib(0, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::position)));
        textVboAttribs.push_back(vertexAttrib(1, 2, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::texCoord)));

        auto textVbo = new vertexBuffer<vertex>(textVboAttribs);
        textVbo->storage(textQuad->vboSize, textQuad->vboData, bufferStorageUsage::dynamic | bufferStorageUsage::write);

        auto textEbo = new buffer<uint>(bufferTarget::element);
        textEbo->storage(textQuad->eboSize, textQuad->eboData, bufferStorageUsage::dynamic | bufferStorageUsage::write);

        vector<vertexAttrib> glyphsIdsAttribs;
        glyphsIdsAttribs.push_back(vertexAttrib(2, 1, GL_UNSIGNED_INT, 0, 0, 1));
        _glyphIdsBuffer = new vertexBuffer<uint>(glyphsIdsAttribs);
        _glyphIdsBuffer->data(sizeof(uint), nullptr, bufferDataUsage::dynamicDraw);

        vector<vertexAttrib> textModelMatricesAttribs;

        for (uint i = 0; i < 4; ++i)
            textModelMatricesAttribs.push_back(vertexAttrib(3 + i, 4, GL_FLOAT, sizeof(mat4), (const void*)(sizeof(GLfloat) * i * 4), 1));

        _modelMatricesBuffer = new vertexBuffer<mat4>(textModelMatricesAttribs);
        _modelMatricesBuffer->data(sizeof(mat4), nullptr, bufferDataUsage::dynamicDraw);

        _glyphRenderDataBuffer = new buffer<glyphRenderData>(bufferTarget::shader);
        _glyphRenderDataBuffer->data(sizeof(glyphRenderData), nullptr, bufferDataUsage::dynamicDraw);

        glBindVertexArray(0);
    }

    void textRenderer::recreateBuffers()
    {
        auto glyphsIds = vector<uint>();
        auto modelMatrices = vector<mat4>();
        auto renderData = vector<glyphRenderData>();

        auto glyphId = 0u;
        for (auto pair : _instances)
        {
            for (auto instance : pair.second->glyphs)
            {
                glyphsIds.push_back(glyphId++);
                modelMatrices.push_back(instance->modelMatrix);
                renderData.push_back(instance->renderData);
            }
        }

        _glyphCount = glyphsIds.size();

        _glyphIdsBuffer->data(sizeof(uint) * _glyphCount, &glyphsIds[0], bufferDataUsage::dynamicDraw);
        _modelMatricesBuffer->data(sizeof(mat4) * _glyphCount, &modelMatrices[0], bufferDataUsage::dynamicDraw);
        _glyphRenderDataBuffer->data(sizeof(glyphRenderData) * _glyphCount, &renderData[0], bufferDataUsage::dynamicDraw);
    }

    textRenderer::textInstance* textRenderer::buildTextInstance(text* text)
    {
        vector<glyphInstance*> glyphInstances;

        auto font = text->getFont();
        auto textString = text->getText();
        auto position = text->getNode()->getTransform()->getPosition();
        auto color = text->getColor();

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
            renderData.position = glyph->texPosition;
            renderData.size = glyph->texSize;
            renderData.shift = shift;
            renderData.unit = glyph->texUnit;
            renderData.page = glyph->texPage;
            renderData.color = color;

            auto glyphInstance = new textRenderer::glyphInstance();
            glyphInstance->modelMatrix = modelMatrix;
            glyphInstance->renderData = renderData;

            glyphInstances.push_back(glyphInstance);

            previousGlyph = glyph;
        }

        auto instance = new textInstance();
        instance->glyphs = glyphInstances;

        return instance;
    }

    void textRenderer::add(text* text)
    {
        _instances[text] = buildTextInstance(text);
        recreateBuffers();
    }

    void textRenderer::remove(text* text)
    {
        _instances.erase(text);
        recreateBuffers();
    }

    void textRenderer::update(text* text)
    {
        _instances[text] = buildTextInstance(text);
        recreateBuffers();
    }

    void textRenderer::render(program* program)
    {
        _glyphRenderDataBuffer->bindBufferBase(1);

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendColor(1, 1, 1, 1);

        program->bind();
        program->setUniform(0, _texelSize);
        program->setUniform(1, _gl->texturesManager->units);

        glBindVertexArray(_vao);
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(_glyphCount));
        glBindVertexArray(0);

        program->unbind();

        glBlendColor(0, 0, 0, 0);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        glEnable(GL_CULL_FACE);
    }
}
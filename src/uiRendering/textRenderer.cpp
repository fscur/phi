#include <precompiled.h>
#include "textRenderer.h"

#include <core\geometry.h>

namespace phi
{
    textRenderer::textRenderer(gl* gl) :
        _gl(gl)
    {
        createBuffers();
    }

    textRenderer::~textRenderer()
    {
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

        _renderDataBuffer = new buffer<glyphRenderData>(bufferTarget::shader);
        _renderDataBuffer->data(sizeof(glyphRenderData), nullptr, bufferDataUsage::dynamicDraw);

        glBindVertexArray(0);
    }

    void textRenderer::add(textRenderData* renderData)
    {
        auto font = renderData->font;
        float baseLine = font->getBaseLine();
        float spacing = font->getSpacing();
        float x = renderData->position.x + spacing;
        float y = renderData->position.y - baseLine + spacing;
        float z = renderData->position.z;

        glyph* previousGlyph = nullptr;
        auto textLength = renderData->text.length();

        auto glyph = _gl->fontsManager->getGlyph(font, (ulong)renderData->text[0]);
        x -= glyph->offsetX;

        for (auto i = 0; i < textLength; i++)
        {
            glyph = _gl->fontsManager->getGlyph(font, (ulong)renderData->text[i]);
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

            _modelMatrices.push_back(modelMatrix);

            x += glyph->horiAdvance + kern.x;

            float shift = std::abs(x0 - static_cast<int>(x0));

            glyphRenderData info;
            info.position = glyph->texPosition;
            info.size = glyph->texSize;
            info.shift = shift;
            info.unit = glyph->texUnit;
            info.page = glyph->texPage;
            info.color = renderData->color;

            _renderData.push_back(info);

            previousGlyph = glyph;
        }

        size_t glyphCount = _renderData.size();
        vector<uint> glyphIds;

        for (uint i = 0u; i < glyphCount; i++)
            glyphIds.push_back(i);

        _glyphIdsBuffer->data(sizeof(uint) * glyphCount, &glyphIds[0], bufferDataUsage::dynamicDraw);
        _modelMatricesBuffer->data(sizeof(mat4) * _modelMatrices.size(), &_modelMatrices[0], bufferDataUsage::dynamicDraw);
        _renderDataBuffer->data(sizeof(glyphRenderData) * glyphCount, &_renderData[0], bufferDataUsage::dynamicDraw);
    }

    void textRenderer::render(program * program)
    {
        _renderDataBuffer->bindBufferBase(1);

        auto texelSize = 1.0f / (float)_gl->fontsManager->getGlyphAtlasSize();

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendColor(1, 1, 1, 1);

        program->bind();
        program->setUniform(0, glm::vec2(texelSize, texelSize));
        program->setUniform(1, _gl->texturesManager->units);

        glBindVertexArray(_vao);
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(_renderData.size()));
        glBindVertexArray(0);

        program->unbind();

        glBlendColor(0, 0, 0, 0);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        glEnable(GL_CULL_FACE);
    }
}
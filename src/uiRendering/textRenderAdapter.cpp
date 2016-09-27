#include <precompiled.h>
#include "textRenderAdapter.h"

#include <core\node.h>
#include <core\geometry.h>
#include <core\notImplementedException.h>
#include <rendering\fontsManager.h>

namespace phi
{
    textRenderAdapter::textRenderAdapter(camera* camera) :
        _camera(camera)
    {
        _cameraChangedEventToken = _camera->getTransform()->getChangedEvent()->assign(std::bind(&textRenderAdapter::onCameraChanged, this, std::placeholders::_1));
        createBuffers();
    }

    void textRenderAdapter::onCameraChanged(transform* transform)
    {
        for (auto pair : _texts)
            if (pair.first->isBillboard())
                updateTransform(pair.first);
    }

    textRenderAdapter::~textRenderAdapter()
    {
        safeDelete(_glyphRenderDataBuffer);
        safeDelete(_vao);
    }

    void textRenderAdapter::createBuffers()
    {
        vector<vertexBufferAttribute> modelMatricesAttribs;

        for (uint i = 0; i < 4; ++i)
            modelMatricesAttribs.push_back(vertexBufferAttribute(2 + i, 4, GL_FLOAT, sizeof(mat4), (const void*)(sizeof(GLfloat) * i * 4), 1));

        _modelMatricesBuffer = new mappedVertexBuffer<text*, mat4>("modelMatrices", modelMatricesAttribs);
        _parentModelMatricesBuffer = new mappedBuffer<text*, mat4>("ParentModelMatricesData", bufferTarget::shader);
        _glyphRenderDataBuffer = new mappedBuffer<text*, glyphRenderData>("GlyphRenderData", bufferTarget::shader);

        auto vertices = vector<vertex>
        {
            vertex(vec3(0.0f, 0.0f, +0.0f), vec2(0.0f, 0.0f)),
            vertex(vec3(1.0f, 0.0f, +0.0f), vec2(1.0f, 0.0f)),
            vertex(vec3(1.0f, -1.0f, +0.0f), vec2(1.0f, 1.0f)),
            vertex(vec3(0.0f, -1.0f, +0.0f), vec2(0.0f, 1.0f))
        };

        auto indices = vector<uint>{ 0, 1, 2, 2, 3, 0 };
        auto textQuad = geometry::create(vertices, indices);

        auto renderFunction = [&]
        {
            glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(_glyphRenderDataBuffer->getInstanceCount()));
        };

        _vao = vertexArrayObject::createQuadVao(textQuad, renderFunction);
        _vao->addBuffer(_modelMatricesBuffer);

        safeDelete(textQuad);
    }

    textInstance textRenderAdapter::createTextInstance(text* text, int parentModelMatrixIndex)
    {
        auto textInstance = phi::textInstance();
        textInstance.parentModelMatrixIndex = parentModelMatrixIndex;

        auto font = text->getFont();
        auto textString = text->getText();

        auto baseLine = font->getBaseLine();
        auto spacing = font->getSpacing();
        auto lineHeight = font->getLineHeight();
        auto x = spacing;
        auto y = -spacing - lineHeight - baseLine;

        glyph* previousGlyph = nullptr;

        auto glyphTextureData = fontsManager::getGlyph(font, (ulong)textString[0]);
        x -= glyphTextureData->glyph->offsetX;

        auto textLength = textString.length();
        for (auto i = 0u; i < textLength; i++)
        {
            glyphTextureData = fontsManager::getGlyph(font, (ulong)textString[i]);
            auto glyph = glyphTextureData->glyph;

            auto kern = font->getKerning(previousGlyph, glyphTextureData->glyph);
            auto w = glyph->width;
            auto h = glyph->height;
            auto x0 = x + glyph->offsetX;
            auto y0 = y + h - (h - glyph->offsetY);

            auto modelMatrix = mat4(
                w, 0.0f, 0.0f, 0.0f,
                0.0f, h, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                x0, y0, 0.0f, 1.0f);

            x += glyph->horiAdvance + kern.x;

            float shift = std::abs(x0 - static_cast<int>(x0));

            auto renderData = glyphRenderData(glyphTextureData, shift, text->getColor());
            renderData.parentModelMatrixIndex = parentModelMatrixIndex;

            textInstance.glyphsRenderData.push_back(renderData);
            textInstance.modelMatrices.push_back(modelMatrix);

            previousGlyph = glyph;
        }

        return textInstance;
    }

    mat4 textRenderAdapter::getModelMatrix(text* text)
    {
        auto textTransform = text->getNode()->getTransform();
        auto position = textTransform->getPosition();
        auto orientation = textTransform->getOrientation();

        mat4 rotationMatrix;
        if (text->isBillboard())
        {
            auto cameraTransform = _camera->getTransform();
            auto cameraUp = cameraTransform->getUp();
            auto cameraRight = cameraTransform->getRight();
            auto cameraDir = cameraTransform->getDirection();

            rotationMatrix = mat4(
                vec4(-cameraRight, 0.0f),
                vec4(cameraUp, 0.0f),
                vec4(-cameraDir, 0.0f),
                vec4(vec3(), 1.0f));
        }
        else
            rotationMatrix = mat4(orientation);

        return glm::translate(position) * rotationMatrix;
    }

    void textRenderAdapter::add(text* text)
    {
        auto parentModelMatrixIndex = static_cast<int>(_texts.size());
        auto textInstance = createTextInstance(text, parentModelMatrixIndex);

        _glyphRenderDataBuffer->addRange(text, &textInstance.glyphsRenderData[0], textInstance.glyphsRenderData.size());
        _modelMatricesBuffer->addRange(text, &textInstance.modelMatrices[0], textInstance.modelMatrices.size());
        _parentModelMatricesBuffer->add(text, getModelMatrix(text));

        textInstance.textChangedEventToken = text->textChangedEvent.assign(std::bind(&textRenderAdapter::updateText, this, std::placeholders::_1));

        _texts[text] = textInstance;
    }

    void textRenderAdapter::remove(text* text)
    {
        _glyphRenderDataBuffer->remove(text);
        _modelMatricesBuffer->remove(text);
        _parentModelMatricesBuffer->remove(text);

        auto textInstance = _texts[text];
        text->textChangedEvent.unassign(textInstance.textChangedEventToken);
        _texts.erase(text);
        updateParentModelMatrixIndicesAfterRemoval(textInstance.parentModelMatrixIndex);
    }

    void textRenderAdapter::updateParentModelMatrixIndicesAfterRemoval(int removedParentModelMatrixIndex)
    {
        for (auto& pair : _texts)
        {
            auto parentModelMatrixIndex = pair.second.parentModelMatrixIndex;
            if (parentModelMatrixIndex >= removedParentModelMatrixIndex)
            {
                pair.second.parentModelMatrixIndex = --parentModelMatrixIndex;
                auto glyphs = pair.second.glyphsRenderData;
                for (auto& glyph : glyphs)
                    glyph.parentModelMatrixIndex = parentModelMatrixIndex;

                _glyphRenderDataBuffer->updateRange(pair.first, &glyphs[0], glyphs.size());
            }
        }
    }

    void textRenderAdapter::updateTransform(text* text)
    {
        _parentModelMatricesBuffer->update(text, getModelMatrix(text));
    }

    void textRenderAdapter::updateText(text* text)
    {
        auto parentModelMatrixIndex = _texts[text].parentModelMatrixIndex;
        auto textInstance = createTextInstance(text, parentModelMatrixIndex);
        _texts[text] = textInstance;

        _glyphRenderDataBuffer->updateRange(text, &textInstance.glyphsRenderData[0], textInstance.glyphsRenderData.size());
        _modelMatricesBuffer->updateRange(text, &textInstance.modelMatrices[0], textInstance.modelMatrices.size());
    }
}
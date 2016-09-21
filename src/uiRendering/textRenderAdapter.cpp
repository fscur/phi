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
        for (auto text : _texts)
            update(text);
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
        _glyphRenderDataBuffer = new mappedBuffer<text*, glyphRenderData>("GlyphRenderData", bufferTarget::shader);

        auto vertices = vector<vertex>
        {
            vertex(vec3(0.0f, 0.0f, +0.0f), vec2(0.0f, 0.0f)),
            vertex(vec3(1.0f, 0.0f, +0.0f), vec2(1.0f, 0.0f)),
            vertex(vec3(1.0f, 1.0f, +0.0f), vec2(1.0f, 1.0f)),
            vertex(vec3(0.0f, 1.0f, +0.0f), vec2(0.0f, 1.0f))
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

    textInstance textRenderAdapter::createTextInstance(text* text)
    {
        auto textInstance = phi::textInstance();

        auto font = text->getFont();
        auto textString = text->getText();
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

        auto parentModelMatrix = glm::translate(position) * rotationMatrix;

        auto baseLine = font->getBaseLine();
        auto spacing = font->getSpacing();
        auto x = spacing;
        auto y = -baseLine + spacing;

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
            auto y0 = y - h + glyph->offsetY;

            auto modelMatrix = mat4(
                w, 0.0f, 0.0f, 0.0f,
                0.0f, -h, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                x0, y0 + h, 0.0f, 1.0f);

            x += glyph->horiAdvance + kern.x;

            float shift = std::abs(x0 - static_cast<int>(x0));

            auto renderData = glyphRenderData(glyphTextureData, shift, text->getColor());

            textInstance.add(renderData);
            textInstance.add(parentModelMatrix * modelMatrix);

            previousGlyph = glyph;
        }

        return textInstance;
    }

    void textRenderAdapter::add(text* text)
    {
        auto textInstance = createTextInstance(text);
        auto modelMatrices = textInstance.getModelMatrices();
        auto glyphsRenderData = textInstance.getGlyphsRenderData();

        _glyphRenderDataBuffer->addRange(text, &glyphsRenderData[0], glyphsRenderData.size());
        _modelMatricesBuffer->addRange(text, &modelMatrices[0], modelMatrices.size());
        _texts.push_back(text);
    }

    void textRenderAdapter::remove(text* text)
    {
        _glyphRenderDataBuffer->remove(text);
        _modelMatricesBuffer->remove(text);
        removeIfContains(_texts, text);
    }

    void textRenderAdapter::update(text* text)
    {
        auto textInstance = createTextInstance(text);
        auto modelMatrices = textInstance.getModelMatrices();
        auto glyphsRenderData = textInstance.getGlyphsRenderData();

        _glyphRenderDataBuffer->updateRange(text, &glyphsRenderData[0], glyphsRenderData.size());
        _modelMatricesBuffer->updateRange(text, &modelMatrices[0], modelMatrices.size());
    }
}
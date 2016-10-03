#include <precompiled.h>
#include "textRenderAdapter.h"

#include <core\node.h>
#include <core\geometry.h>
#include <core\notImplementedException.h>
#include <rendering\fontsManager.h>

namespace phi
{
    textRenderAdapter::textRenderAdapter()
    {
        createBuffers();
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
        auto textQuad = geometry::create(guid::newGuid(), vertices, indices);

        auto renderFunction = [&]
        {
            glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(_glyphRenderDataBuffer->getInstanceCount()));
        };

        _vao = vertexArrayObject::createQuadVao(textQuad, renderFunction);
        _vao->addBuffer(_modelMatricesBuffer);

        safeDelete(textQuad);
    }

    void textRenderAdapter::add(text* text)
    {
        auto textInstance = textInstance::from(text);
        auto modelMatrices = textInstance.getModelMatrices();
        auto glyphsRenderData = textInstance.getGlyphsRenderData();

        _glyphRenderDataBuffer->addRange(text, &glyphsRenderData[0], glyphsRenderData.size());
        _modelMatricesBuffer->addRange(text, &modelMatrices[0], modelMatrices.size());
    }

    void textRenderAdapter::remove(text* text)
    {
        _glyphRenderDataBuffer->remove(text);
        _modelMatricesBuffer->remove(text);
    }

    void textRenderAdapter::update(text* text)
    {
        auto textInstance = textInstance::from(text);
        auto modelMatrices = textInstance.getModelMatrices();
        auto glyphsRenderData = textInstance.getGlyphsRenderData();

        _glyphRenderDataBuffer->updateRange(text, &glyphsRenderData[0], glyphsRenderData.size());
        _modelMatricesBuffer->updateRange(text, &modelMatrices[0], modelMatrices.size());
    }
}
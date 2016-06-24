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
        safeDelete(_modelMatricesBuffer);
        safeDelete(_glyphRenderDataBuffer);
    }

    void textRenderAdapter::createBuffers()
    {
        _glyphRenderDataBuffer = new mappedBuffer<text*, glyphRenderData>("GlyphRenderData", bufferTarget::shader, 1000);

        auto vertices = vector<vertex>
        {
            vertex(vec3(0.0f, 0.0f, +0.0f), vec2(0.0f, 0.0f)),
            vertex(vec3(1.0f, 0.0f, +0.0f), vec2(1.0f, 0.0f)),
            vertex(vec3(1.0f, 1.0f, +0.0f), vec2(1.0f, 1.0f)),
            vertex(vec3(0.0f, 1.0f, +0.0f), vec2(0.0f, 1.0f))
        };
        auto indices = vector<uint>{ 0, 1, 2, 2, 3, 0 };
        auto textQuad = geometry::create(vertices, indices);

        vector<vertexBufferAttribute> vboAttribs;
        vboAttribs.push_back(vertexBufferAttribute(0, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::position)));
        vboAttribs.push_back(vertexBufferAttribute(1, 2, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::texCoord)));

        auto vbo = new vertexBuffer("vbo", vboAttribs);
        vbo->storage(textQuad->vboSize, textQuad->vboData, bufferStorageUsage::dynamic | bufferStorageUsage::write);

        auto ebo = new buffer("ebo", bufferTarget::element);
        ebo->storage(textQuad->eboSize, textQuad->eboData, bufferStorageUsage::dynamic | bufferStorageUsage::write);

        vector<vertexBufferAttribute> modelMatricesAttribs;

        for (uint i = 0; i < 4; ++i)
            modelMatricesAttribs.push_back(vertexBufferAttribute(2 + i, 4, GL_FLOAT, sizeof(mat4), (const void*)(sizeof(GLfloat) * i * 4), 1));

        _modelMatricesBuffer = new mappedVertexBuffer<text*, mat4>("modelMatrices", modelMatricesAttribs, 1000);

        _vao = new vertexArrayObject();
        _vao->add(vbo);
        _vao->add(_modelMatricesBuffer);
        _vao->setEbo(ebo);

        _vao->setOnRender([=]
        {
            glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(_glyphRenderDataBuffer->getInstanceCount()));
        });
    }

    void textRenderAdapter::add(text* text)
    {
        auto textInstance = textInstance::from(text);
        auto modelMatrices = textInstance->getModelMatrices();
        auto glyphsRenderData = textInstance->getGlyphsRenderData();

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
        auto modelMatrices = textInstance->getModelMatrices();
        auto glyphsRenderData = textInstance->getGlyphsRenderData();

        _glyphRenderDataBuffer->updateRange(text, &glyphsRenderData[0], glyphsRenderData.size());
        _modelMatricesBuffer->updateRange(text, &modelMatrices[0], modelMatrices.size());
    }
}
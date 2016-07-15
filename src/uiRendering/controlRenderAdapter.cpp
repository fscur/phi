#include <precompiled.h>
#include "controlRenderAdapter.h"

#include <core\notImplementedException.h>
#include <core\geometry.h>
#include <core\node.h>
#include <rendering\texturesManager.h>

namespace phi
{
    controlRenderAdapter::controlRenderAdapter()
    {
        createVao();
        createControlRenderDataBuffer();
    }

    controlRenderAdapter::~controlRenderAdapter()
    {
        safeDelete(_renderDataBuffer);
        safeDelete(_vao);
    }

    void controlRenderAdapter::createVao()
    {
        vector<vertexBufferAttribute> modelMatricesAttribs;
        for (uint i = 0; i < 4; ++i)
            modelMatricesAttribs.push_back(vertexBufferAttribute(2 + i, 4, GL_FLOAT, sizeof(mat4), (const void*)(sizeof(GLfloat) * i * 4), 1));

        _modelMatricesBuffer = new mappedVertexBuffer<control*, mat4>("modelMatrices", modelMatricesAttribs);

        auto renderFunction = [&]
        {
            glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(_modelMatricesBuffer->getInstanceCount()));
        };

        auto controlQuad = createControlQuad();

        _vao = vertexArrayObject::createQuadVao(controlQuad, renderFunction);
        _vao->addBuffer(_modelMatricesBuffer);

        safeDelete(controlQuad);
    }

    geometry* controlRenderAdapter::createControlQuad()
    {
        auto vertices = vector<vertex>
        {
            vertex(vec3(0.0f, 0.0f, +0.0f), vec2(0.0f, 0.0f)),
            vertex(vec3(1.0f, 0.0f, +0.0f), vec2(1.0f, 0.0f)),
            vertex(vec3(1.0f, 1.0f, +0.0f), vec2(1.0f, 1.0f)),
            vertex(vec3(0.0f, 1.0f, +0.0f), vec2(0.0f, 1.0f))
        };

        auto indices = vector<uint>{ 0, 1, 2, 2, 3, 0 };
        return geometry::create(vertices, indices);
    }

    void controlRenderAdapter::createControlRenderDataBuffer()
    {
        _renderDataBuffer = new mappedBuffer<control*, controlRenderData>("ControlRenderDataBuffer", bufferTarget::shader);
    }

    void controlRenderAdapter::updateModelMatrix(control* control)
    {
        auto modelMatrix = control->getNode()->getTransform()->getModelMatrix();
        _modelMatricesBuffer->update(control, modelMatrix);
    }

    void controlRenderAdapter::updateControlRenderData(control* control)
    {
        auto renderData = controlRenderData::from(control);
        _renderDataBuffer->update(control, renderData);
    }

    void controlRenderAdapter::add(control* control)
    {
        auto renderData = controlRenderData::from(control);
        _renderDataBuffer->add(control, renderData);

        auto modelMatrix = control->getNode()->getTransform()->getModelMatrix();
        _modelMatricesBuffer->add(control, modelMatrix);
    }

    void controlRenderAdapter::remove(control* control)
    {
        _renderDataBuffer->remove(control);
        _modelMatricesBuffer->remove(control);
    }

    void controlRenderAdapter::update(control* control)
    {
        updateModelMatrix(control);
        updateControlRenderData(control);
    }
}
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
        createBuffers();
    }

    controlRenderAdapter::~controlRenderAdapter()
    {
        safeDelete(_modelMatricesBuffer);
        safeDelete(_renderDataBuffer);
    }

    void controlRenderAdapter::createBuffers()
    {
        vector<vertexBufferAttribute> modelMatricesAttribs;
        for (uint i = 0; i < 4; ++i)
            modelMatricesAttribs.push_back(vertexBufferAttribute(2 + i, 4, GL_FLOAT, sizeof(mat4), (const void*)(sizeof(GLfloat) * i * 4), 1));

        _modelMatricesBuffer = new mappedVertexBuffer<control*, mat4>("modelMatrices", modelMatricesAttribs, MAX_CONTROL_INSTANCES);
        _renderDataBuffer = new mappedBuffer<control*, controlRenderData>("ControlRenderDataBuffer", bufferTarget::shader, MAX_CONTROL_INSTANCES);

        auto vertices = vector<vertex>
        {
            vertex(vec3(0.0f, 0.0f, +0.0f), vec2(0.0f, 0.0f)),
            vertex(vec3(1.0f, 0.0f, +0.0f), vec2(1.0f, 0.0f)),
            vertex(vec3(1.0f, 1.0f, +0.0f), vec2(1.0f, 1.0f)),
            vertex(vec3(0.0f, 1.0f, +0.0f), vec2(0.0f, 1.0f))
        };

        auto indices = vector<uint>{ 0, 1, 2, 2, 3, 0 };
        auto controlQuad = geometry::create(vertices, indices);

        auto renderFunction = [&]
        {
            glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(_renderDataBuffer->getInstanceCount()));
        };

        _vao = vertexArrayObject::createQuadVao(controlQuad, renderFunction);
        _vao->add(_modelMatricesBuffer);
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
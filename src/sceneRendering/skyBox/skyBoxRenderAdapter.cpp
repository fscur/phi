#include <precompiled.h>
#include "skyBoxRenderAdapter.h"

#include <core\node.h>
#include <core\geometry.h>
#include <core\notImplementedException.h>

namespace phi
{
    skyBoxRenderAdapter::skyBoxRenderAdapter()
    {
        createBuffers();
    }

    skyBoxRenderAdapter::~skyBoxRenderAdapter()
    {
        safeDelete(_skyBoxRenderDataBuffer);
        safeDelete(_vao);
    }

    void skyBoxRenderAdapter::createBuffers()
    {
        vector<vertexBufferAttribute> modelMatricesAttribs;

        for (uint i = 0; i < 4; ++i)
            modelMatricesAttribs.push_back(vertexBufferAttribute(2 + i, 4, GL_FLOAT, sizeof(mat4), (const void*)(sizeof(GLfloat) * i * 4), 1));

        _modelMatricesBuffer = new mappedVertexBuffer<skyBox*, mat4>("modelMatrices", modelMatricesAttribs);
        _skyBoxRenderDataBuffer = new mappedBuffer<skyBox*, skyBoxRenderData>("SkyBoxRenderDataBuffer", bufferTarget::shader);

        auto vertices = vector<vertex>
        {
            vertex(vec3(+0.5f, +0.5f, +0.5f)), //0
            vertex(vec3(+0.5f, -0.5f, +0.5f)), //1
            vertex(vec3(-0.5f, -0.5f, +0.5f)), //2
            vertex(vec3(-0.5f, +0.5f, +0.5f)), //3
            vertex(vec3(+0.5f, +0.5f, -0.5f)), //4
            vertex(vec3(+0.5f, -0.5f, -0.5f)), //5
            vertex(vec3(-0.5f, -0.5f, -0.5f)), //6
            vertex(vec3(-0.5f, +0.5f, -0.5f)) //7
        };

        auto indices = vector<uint>
        { 
            0, 1, 2, 2, 3, 0,
            4, 5, 1, 1, 0, 4,
            7, 6, 5, 5, 4, 7,
            3, 2, 6, 6, 7, 3,
            4, 0, 3, 3, 7, 4,
            1, 5, 6, 6, 2, 1
        };

        auto skyBoxQuad = geometry::create(guid::newGuid(), vertices, indices);

        auto renderFunction = [&]
        {
            glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(_skyBoxRenderDataBuffer->getInstanceCount()));
        };

        _vao = vertexArrayObject::createQuadVao(skyBoxQuad, renderFunction);
        _vao->addBuffer(_modelMatricesBuffer);

        safeDelete(skyBoxQuad);
    }

    void skyBoxRenderAdapter::add(skyBox* skyBox)
    {
        auto renderData = skyBoxRenderData::from(skyBox);
        _skyBoxRenderDataBuffer->add(skyBox, renderData);

        auto modelMatrix = skyBox->getNode()->getTransform()->getModelMatrix();
        _modelMatricesBuffer->add(skyBox, modelMatrix);
    }

    void skyBoxRenderAdapter::remove(skyBox* skyBox)
    {
        _skyBoxRenderDataBuffer->remove(skyBox);
        _modelMatricesBuffer->remove(skyBox);
    }

    void skyBoxRenderAdapter::update(skyBox* skyBox)
    {
        auto renderData = skyBoxRenderData::from(skyBox);
        _skyBoxRenderDataBuffer->update(skyBox, renderData);

        auto modelMatrix = skyBox->getNode()->getTransform()->getModelMatrix();
        _modelMatricesBuffer->update(skyBox, modelMatrix);
    }
}
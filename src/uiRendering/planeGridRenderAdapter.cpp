#include <precompiled.h>
#include "planeGridRenderAdapter.h"

#include <core\notImplementedException.h>
#include <core\geometry.h>
#include <core\node.h>
#include <rendering\texturesManager.h>

namespace phi
{
    planeGridRenderAdapter::planeGridRenderAdapter()
    {
        createVao();
        createPlaneGridRenderDataBuffer();
    }

    planeGridRenderAdapter::~planeGridRenderAdapter()
    {
        safeDelete(_vao);
    }

    void planeGridRenderAdapter::createVao()
    {
        _modelMatricesBuffer = new mappedVertexBuffer<planeGrid*, mat4>("modelMatrices",
        {
            vertexBufferAttribute(2, 4, GL_FLOAT, sizeof(mat4), (const void *)(sizeof(GLfloat) * 0 * 4), 1), // I'm not dumb (I think), I just trust the compiler
            vertexBufferAttribute(3, 4, GL_FLOAT, sizeof(mat4), (const void *)(sizeof(GLfloat) * 1 * 4), 1), // I'm not dumb (I think), I just trust the compiler
            vertexBufferAttribute(4, 4, GL_FLOAT, sizeof(mat4), (const void *)(sizeof(GLfloat) * 2 * 4), 1), // I'm not dumb (I think), I just trust the compiler
            vertexBufferAttribute(5, 4, GL_FLOAT, sizeof(mat4), (const void *)(sizeof(GLfloat) * 3 * 4), 1)  // I'm not dumb (I think), I just trust the compiler
        });

        auto renderFunction = [&]
        {
            glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(_modelMatricesBuffer->getInstanceCount()));
        };

        auto quad = createPlaneQuad();
        _vao = vertexArrayObject::createQuadVao(quad, renderFunction);
        _vao->addBuffer(_modelMatricesBuffer);

        safeDelete(quad);
    }

    geometry* planeGridRenderAdapter::createPlaneQuad()
    {
        auto vertices = vector<vertex>
        {
            vertex(vec3(-0.5f, -0.5f, 0.0f), vec2(0.0f, 0.0f)),
            vertex(vec3(+0.5f, -0.5f, 0.0f), vec2(1.0f, 0.0f)),
            vertex(vec3(+0.5f, +0.5f, 0.0f), vec2(1.0f, 1.0f)),
            vertex(vec3(-0.5f, +0.5f, 0.0f), vec2(0.0f, 1.0f))
        };

        auto indices = vector<uint>{ 0, 1, 2, 2, 3, 0 };
        return geometry::create(vertices, indices);
    }

    void planeGridRenderAdapter::createPlaneGridRenderDataBuffer()
    {
        _planeGridRenderDataBuffer = new mappedBuffer<planeGrid*, planeGridRenderData>("PlaneGridRenderDataBuffer", bufferTarget::shader);
     }

    void planeGridRenderAdapter::updateModelMatrix(planeGrid* planeGrid)
    {
        auto modelMatrix = planeGrid->getNode()->getTransform()->getModelMatrix();
        _modelMatricesBuffer->update(planeGrid, modelMatrix);
    }

    void planeGridRenderAdapter::add(planeGrid* planeGrid)
    {
        auto modelMatrix = planeGrid->getNode()->getTransform()->getModelMatrix();
        _modelMatricesBuffer->add(planeGrid, modelMatrix);

        auto planeGridRenderData = planeGridRenderData::from(planeGrid);
        _planeGridRenderDataBuffer->add(planeGrid, planeGridRenderData);
    }

    void planeGridRenderAdapter::remove(planeGrid* planeGrid)
    {
        _modelMatricesBuffer->remove(planeGrid);
    }

    void planeGridRenderAdapter::update(planeGrid* planeGrid)
    {
        updateModelMatrix(planeGrid);
    }
}
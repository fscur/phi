#include <precompiled.h>

#include <core/geometry.h>
#include <core/node.h>
#include <core/notImplementedException.h>

#include <rendering/texturesManager.h>

#include "planeGridRenderAdapter.h"

namespace phi
{
    planeGridRenderAdapter::planeGridRenderAdapter() :
        _planeGridEventTokens(unordered_map<planeGrid*, planeGridEventTokens>())
    {
        createVao();
        createPlaneGridRenderDataBuffer();
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
            auto instanceCount = _modelMatricesBuffer->getInstanceCount();
            glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(instanceCount));
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
            vertex(vec3(-0.5f, -0.5f, 0.0f), vec2(-0.5f, -0.5f)),
            vertex(vec3(+0.5f, -0.5f, 0.0f), vec2(+0.5f, -0.5f)),
            vertex(vec3(+0.5f, +0.5f, 0.0f), vec2(+0.5f, +0.5f)),
            vertex(vec3(-0.5f, +0.5f, 0.0f), vec2(-0.5f, +0.5f))
        };

        auto indices = vector<uint>{ 0, 1, 2, 2, 3, 0 };
        return geometry::create(guid::newGuid(), vertices, indices);
    }

    void planeGridRenderAdapter::createPlaneGridRenderDataBuffer()
    {
        _planeGridRenderDataBuffer = new mappedBuffer<planeGrid*, planeGridRenderData>("PlaneGridRenderDataBuffer", bufferTarget::shader);
    }

    planeGridRenderAdapter::~planeGridRenderAdapter()
    {
        safeDelete(_vao);
        safeDelete(_planeGridRenderDataBuffer);
    }

    void planeGridRenderAdapter::add(planeGrid* planeGrid)
    {
        if (planeGrid->isVisible())
            addPlaneGridToBuffers(planeGrid);

        assignChangedEvents(planeGrid);
    }

    void planeGridRenderAdapter::addPlaneGridToBuffers(planeGrid* planeGrid)
    {
        auto modelMatrix = planeGrid->getNode()->getTransform()->getModelMatrix();
        _modelMatricesBuffer->add(planeGrid, modelMatrix);

        auto planeGridRenderData = planeGridRenderData::from(planeGrid);
        _planeGridRenderDataBuffer->add(planeGrid, planeGridRenderData);
    }

    void planeGridRenderAdapter::assignChangedEvents(planeGrid* planeGrid)
    {
        auto colorChangedToken = planeGrid->getColorChanged()->assign(std::bind(&planeGridRenderAdapter::updateRenderData, this, std::placeholders::_1));
        auto lineThicknessChangedToken = planeGrid->getLineThicknessChanged()->assign(std::bind(&planeGridRenderAdapter::updateRenderData, this, std::placeholders::_1));
        auto opacityChangedToken = planeGrid->getOpacityChanged()->assign(std::bind(&planeGridRenderAdapter::updateRenderData, this, std::placeholders::_1));
        auto visibleChangedToken = planeGrid->getVisibleChanged()->assign(std::bind(&planeGridRenderAdapter::planeGridVisibleChanged, this, std::placeholders::_1));
        auto clippingPlanesChangedToken = planeGrid->getClippingPlanesChanged()->assign(std::bind(&planeGridRenderAdapter::updateRenderData, this, std::placeholders::_1));

        auto tokens = planeGridEventTokens();
        tokens.colorChangedEventToken = colorChangedToken;
        tokens.lineThicknessChangedEventToken = lineThicknessChangedToken;
        tokens.opacityChangedEventToken = opacityChangedToken;
        tokens.visibleChangedEventToken = visibleChangedToken;
        tokens.clippingPlanesChangedEventToken = clippingPlanesChangedToken;

        _planeGridEventTokens[planeGrid] = tokens;
    }

    void planeGridRenderAdapter::planeGridVisibleChanged(planeGrid* planeGrid)
    {
        if (planeGrid->isVisible())
            addPlaneGridToBuffers(planeGrid);
        else
            removePlaneGridFromBuffers(planeGrid);
    }

    void planeGridRenderAdapter::remove(planeGrid* planeGrid)
    {
        if (planeGrid->isVisible())
            removePlaneGridFromBuffers(planeGrid);

        unassignChangedEvents(planeGrid);
    }

    void planeGridRenderAdapter::removePlaneGridFromBuffers(planeGrid* planeGrid)
    {
        _modelMatricesBuffer->remove(planeGrid);
        _planeGridRenderDataBuffer->remove(planeGrid);
    }

    void planeGridRenderAdapter::unassignChangedEvents(planeGrid* planeGrid)
    {
        auto tokens = _planeGridEventTokens[planeGrid];

        planeGrid->getColorChanged()->unassign(tokens.colorChangedEventToken);
        planeGrid->getLineThicknessChanged()->unassign(tokens.lineThicknessChangedEventToken);
        planeGrid->getOpacityChanged()->unassign(tokens.opacityChangedEventToken);
        planeGrid->getVisibleChanged()->unassign(tokens.visibleChangedEventToken);
        planeGrid->getClippingPlanesChanged()->unassign(tokens.clippingPlanesChangedEventToken);

        _planeGridEventTokens.erase(planeGrid);
    }

    void planeGridRenderAdapter::update(planeGrid* planeGrid)
    {
        updateModelMatrix(planeGrid);
        updateRenderData(planeGrid);
    }

    void planeGridRenderAdapter::updateModelMatrix(planeGrid* planeGrid)
    {
        if (!planeGrid->isVisible())
            return;

        auto modelMatrix = planeGrid->getNode()->getTransform()->getModelMatrix();
        _modelMatricesBuffer->update(planeGrid, modelMatrix);
    }

    void planeGridRenderAdapter::updateRenderData(planeGrid* planeGrid)
    {
        if (!planeGrid->isVisible())
            return;

        auto planeGridRenderData = planeGridRenderData::from(planeGrid);

        _planeGridRenderDataBuffer->update(planeGrid, planeGridRenderData);
    }
}
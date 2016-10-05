#include <precompiled.h>

#include <core/geometry.h>
#include <core/node.h>
#include <core/notImplementedException.h>

#include <rendering/texturesManager.h>

#include "rotationPlaneGridRenderAdapter.h"

namespace phi
{
    rotationPlaneGridRenderAdapter::rotationPlaneGridRenderAdapter() :
        _planeGridEventTokens(unordered_map<rotationPlaneGrid*, rotationPlaneGridEventTokens>())
    {
        createVao();
        createPlaneGridRenderDataBuffer();
    }

    void rotationPlaneGridRenderAdapter::createVao()
    {
        _modelMatricesBuffer = new mappedVertexBuffer<rotationPlaneGrid*, mat4>("modelMatrices",
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

    geometry* rotationPlaneGridRenderAdapter::createPlaneQuad()
    {
        auto vertices = vector<vertex>
        {
            vertex(vec3(-0.5f, -0.5f, 0.0f), vec2(-0.5f, -0.5f)),
            vertex(vec3(+0.5f, -0.5f, 0.0f), vec2(+0.5f, -0.5f)),
            vertex(vec3(+0.5f, +0.5f, 0.0f), vec2(+0.5f, +0.5f)),
            vertex(vec3(-0.5f, +0.5f, 0.0f), vec2(-0.5f, +0.5f))
        };

        auto indices = vector<uint>{ 0, 1, 2, 2, 3, 0 };
        return geometry::create(vertices, indices);
    }

    void rotationPlaneGridRenderAdapter::createPlaneGridRenderDataBuffer()
    {
        _planeGridRenderDataBuffer = new mappedBuffer<rotationPlaneGrid*, rotationPlaneGridRenderData>("RotationPlaneGridRenderDataBuffer", bufferTarget::shader);
    }

    rotationPlaneGridRenderAdapter::~rotationPlaneGridRenderAdapter()
    {
        safeDelete(_vao);
        safeDelete(_planeGridRenderDataBuffer);
    }

    void rotationPlaneGridRenderAdapter::add(rotationPlaneGrid* rotationPlaneGrid)
    {
        if (rotationPlaneGrid->isVisible())
            addPlaneGridToBuffers(rotationPlaneGrid);

        assignChangedEvents(rotationPlaneGrid);
    }

    void rotationPlaneGridRenderAdapter::addPlaneGridToBuffers(rotationPlaneGrid* rotationPlaneGrid)
    {
        auto modelMatrix = rotationPlaneGrid->getNode()->getTransform()->getModelMatrix();
        _modelMatricesBuffer->add(rotationPlaneGrid, modelMatrix);

        auto planeGridRenderData = rotationPlaneGridRenderData::from(rotationPlaneGrid);
        _planeGridRenderDataBuffer->add(rotationPlaneGrid, planeGridRenderData);
    }

    void rotationPlaneGridRenderAdapter::assignChangedEvents(rotationPlaneGrid* rotationPlaneGrid)
    {
        auto colorChangedToken = rotationPlaneGrid->getColorChanged()->assign(std::bind(&rotationPlaneGridRenderAdapter::updateRenderData, this, std::placeholders::_1));
        auto lineThicknessChangedToken = rotationPlaneGrid->getLineThicknessChanged()->assign(std::bind(&rotationPlaneGridRenderAdapter::updateRenderData, this, std::placeholders::_1));
        auto opacityChangedToken = rotationPlaneGrid->getOpacityChanged()->assign(std::bind(&rotationPlaneGridRenderAdapter::updateRenderData, this, std::placeholders::_1));
        auto visibleChangedToken = rotationPlaneGrid->getVisibleChanged()->assign(std::bind(&rotationPlaneGridRenderAdapter::planeGridVisibleChanged, this, std::placeholders::_1));
        auto clippingPlanesChangedToken = rotationPlaneGrid->getClippingPlanesChanged()->assign(std::bind(&rotationPlaneGridRenderAdapter::updateRenderData, this, std::placeholders::_1));
        auto filledAngleChangedToken = rotationPlaneGrid->getFilledAngleChanged()->assign(std::bind(&rotationPlaneGridRenderAdapter::updateRenderData, this, std::placeholders::_1));

        auto tokens = rotationPlaneGridEventTokens();
        tokens.colorChangedEventToken = colorChangedToken;
        tokens.lineThicknessChangedEventToken = lineThicknessChangedToken;
        tokens.opacityChangedEventToken = opacityChangedToken;
        tokens.visibleChangedEventToken = visibleChangedToken;
        tokens.clippingPlanesChangedEventToken = clippingPlanesChangedToken;
        tokens.filledAngleChangedEventToken = filledAngleChangedToken;

        _planeGridEventTokens[rotationPlaneGrid] = tokens;
    }

    void rotationPlaneGridRenderAdapter::planeGridVisibleChanged(rotationPlaneGrid* rotationPlaneGrid)
    {
        if (rotationPlaneGrid->isVisible())
            addPlaneGridToBuffers(rotationPlaneGrid);
        else
            removePlaneGridFromBuffers(rotationPlaneGrid);
    }

    void rotationPlaneGridRenderAdapter::remove(rotationPlaneGrid* rotationPlaneGrid)
    {
        if (rotationPlaneGrid->isVisible())
            removePlaneGridFromBuffers(rotationPlaneGrid);

        unassignChangedEvents(rotationPlaneGrid);
    }

    void rotationPlaneGridRenderAdapter::removePlaneGridFromBuffers(rotationPlaneGrid* rotationPlaneGrid)
    {
        _modelMatricesBuffer->remove(rotationPlaneGrid);
        _planeGridRenderDataBuffer->remove(rotationPlaneGrid);
    }

    void rotationPlaneGridRenderAdapter::unassignChangedEvents(rotationPlaneGrid* rotationPlaneGrid)
    {
        auto tokens = _planeGridEventTokens[rotationPlaneGrid];

        rotationPlaneGrid->getColorChanged()->unassign(tokens.colorChangedEventToken);
        rotationPlaneGrid->getLineThicknessChanged()->unassign(tokens.lineThicknessChangedEventToken);
        rotationPlaneGrid->getOpacityChanged()->unassign(tokens.opacityChangedEventToken);
        rotationPlaneGrid->getVisibleChanged()->unassign(tokens.visibleChangedEventToken);
        rotationPlaneGrid->getClippingPlanesChanged()->unassign(tokens.clippingPlanesChangedEventToken);
        rotationPlaneGrid->getFilledAngleChanged()->unassign(tokens.filledAngleChangedEventToken);

        _planeGridEventTokens.erase(rotationPlaneGrid);
    }

    void rotationPlaneGridRenderAdapter::update(rotationPlaneGrid* rotationPlaneGrid)
    {
        updateModelMatrix(rotationPlaneGrid);
        updateRenderData(rotationPlaneGrid);
    }

    void rotationPlaneGridRenderAdapter::updateModelMatrix(rotationPlaneGrid* rotationPlaneGrid)
    {
        if (!rotationPlaneGrid->isVisible())
            return;

        auto modelMatrix = rotationPlaneGrid->getNode()->getTransform()->getModelMatrix();
        _modelMatricesBuffer->update(rotationPlaneGrid, modelMatrix);
    }

    void rotationPlaneGridRenderAdapter::updateRenderData(rotationPlaneGrid* rotationPlaneGrid)
    {
        if (!rotationPlaneGrid->isVisible())
            return;

        auto planeGridRenderData = rotationPlaneGridRenderData::from(rotationPlaneGrid);

        _planeGridRenderDataBuffer->update(rotationPlaneGrid, planeGridRenderData);
    }
}
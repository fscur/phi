#include <precompiled.h>

#include <core/geometry.h>
#include <core/node.h>
#include <core/notImplementedException.h>

#include <rendering/texturesManager.h>

#include "translationPlaneGridRenderAdapter.h"

namespace phi
{
    translationPlaneGridRenderAdapter::translationPlaneGridRenderAdapter() :
        _planeGridEventTokens(unordered_map<translationPlaneGrid*, translationPlaneGridEventTokens>())
    {
        createVao();
        createPlaneGridRenderDataBuffer();
    }

    void translationPlaneGridRenderAdapter::createVao()
    {
        _modelMatricesBuffer = new mappedVertexBuffer<translationPlaneGrid*, mat4>("modelMatrices",
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

    geometry* translationPlaneGridRenderAdapter::createPlaneQuad()
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

    void translationPlaneGridRenderAdapter::createPlaneGridRenderDataBuffer()
    {
        _planeGridRenderDataBuffer = new mappedBuffer<translationPlaneGrid*, translationPlaneGridRenderData>("PlaneGridRenderDataBuffer", bufferTarget::shader);
    }

    translationPlaneGridRenderAdapter::~translationPlaneGridRenderAdapter()
    {
        safeDelete(_vao);
        safeDelete(_planeGridRenderDataBuffer);
    }

    void translationPlaneGridRenderAdapter::add(translationPlaneGrid* planeGrid)
    {
        if (planeGrid->isVisible())
            addPlaneGridToBuffers(planeGrid);

        assignChangedEvents(planeGrid);
    }

    void translationPlaneGridRenderAdapter::addPlaneGridToBuffers(translationPlaneGrid* planeGrid)
    {
        auto modelMatrix = planeGrid->getNode()->getTransform()->getModelMatrix();
        _modelMatricesBuffer->add(planeGrid, modelMatrix);

        auto planeGridRenderData = translationPlaneGridRenderData::from(planeGrid);
        _planeGridRenderDataBuffer->add(planeGrid, planeGridRenderData);
    }

    void translationPlaneGridRenderAdapter::assignChangedEvents(translationPlaneGrid* planeGrid)
    {
        auto colorChangedToken = planeGrid->getColorChanged()->assign(std::bind(&translationPlaneGridRenderAdapter::updateRenderData, this, std::placeholders::_1));
        auto lineThicknessChangedToken = planeGrid->getLineThicknessChanged()->assign(std::bind(&translationPlaneGridRenderAdapter::updateRenderData, this, std::placeholders::_1));
        auto opacityChangedToken = planeGrid->getOpacityChanged()->assign(std::bind(&translationPlaneGridRenderAdapter::updateRenderData, this, std::placeholders::_1));
        auto visibleChangedToken = planeGrid->getVisibleChanged()->assign(std::bind(&translationPlaneGridRenderAdapter::planeGridVisibleChanged, this, std::placeholders::_1));
        auto clippingPlanesChangedToken = planeGrid->getClippingPlanesChanged()->assign(std::bind(&translationPlaneGridRenderAdapter::updateRenderData, this, std::placeholders::_1));

        auto tokens = translationPlaneGridEventTokens();
        tokens.colorChangedEventToken = colorChangedToken;
        tokens.lineThicknessChangedEventToken = lineThicknessChangedToken;
        tokens.opacityChangedEventToken = opacityChangedToken;
        tokens.visibleChangedEventToken = visibleChangedToken;
        tokens.clippingPlanesChangedEventToken = clippingPlanesChangedToken;

        _planeGridEventTokens[planeGrid] = tokens;
    }

    void translationPlaneGridRenderAdapter::planeGridVisibleChanged(translationPlaneGrid* planeGrid)
    {
        if (planeGrid->isVisible())
            addPlaneGridToBuffers(planeGrid);
        else
            removePlaneGridFromBuffers(planeGrid);
    }

    void translationPlaneGridRenderAdapter::remove(translationPlaneGrid* planeGrid)
    {
        if (planeGrid->isVisible())
            removePlaneGridFromBuffers(planeGrid);

        unassignChangedEvents(planeGrid);
    }

    void translationPlaneGridRenderAdapter::removePlaneGridFromBuffers(translationPlaneGrid* planeGrid)
    {
        _modelMatricesBuffer->remove(planeGrid);
        _planeGridRenderDataBuffer->remove(planeGrid);
    }

    void translationPlaneGridRenderAdapter::unassignChangedEvents(translationPlaneGrid* planeGrid)
    {
        auto tokens = _planeGridEventTokens[planeGrid];

        planeGrid->getColorChanged()->unassign(tokens.colorChangedEventToken);
        planeGrid->getLineThicknessChanged()->unassign(tokens.lineThicknessChangedEventToken);
        planeGrid->getOpacityChanged()->unassign(tokens.opacityChangedEventToken);
        planeGrid->getVisibleChanged()->unassign(tokens.visibleChangedEventToken);
        planeGrid->getClippingPlanesChanged()->unassign(tokens.clippingPlanesChangedEventToken);

        _planeGridEventTokens.erase(planeGrid);
    }

    void translationPlaneGridRenderAdapter::update(translationPlaneGrid* planeGrid)
    {
        updateModelMatrix(planeGrid);
        updateRenderData(planeGrid);
    }

    void translationPlaneGridRenderAdapter::updateModelMatrix(translationPlaneGrid* planeGrid)
    {
        if (!planeGrid->isVisible())
            return;

        auto modelMatrix = planeGrid->getNode()->getTransform()->getModelMatrix();
        _modelMatricesBuffer->update(planeGrid, modelMatrix);
    }

    void translationPlaneGridRenderAdapter::updateRenderData(translationPlaneGrid* planeGrid)
    {
        if (!planeGrid->isVisible())
            return;

        auto planeGridRenderData = translationPlaneGridRenderData::from(planeGrid);

        _planeGridRenderDataBuffer->update(planeGrid, planeGridRenderData);
    }
}
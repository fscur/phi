#pragma once

#include <phi.h>

#include <core/planeGrid.h>

#include <rendering/indexBuffer.h>
#include <rendering/mappedVertexBuffer.h>
#include <rendering/vertexArrayObject.h>
#include <rendering/vertexBuffer.h>

#include "sceneRenderingApi.h"
#include "rotationPlaneGridRenderData.h"

namespace phi
{
    class rotationPlaneGridRenderAdapter
    {
        struct rotationPlaneGridEventTokens
        {
            eventToken colorChangedEventToken;
            eventToken lineThicknessChangedEventToken;
            eventToken opacityChangedEventToken;
            eventToken visibleChangedEventToken;
            eventToken clippingPlanesChangedEventToken;
        };

    public:
        SCENE_RENDERING_API rotationPlaneGridRenderAdapter();
        SCENE_RENDERING_API ~rotationPlaneGridRenderAdapter();

        SCENE_RENDERING_API void add(rotationPlaneGrid* rotationPlaneGrid);
        SCENE_RENDERING_API void remove(rotationPlaneGrid* rotationPlaneGrid);
        SCENE_RENDERING_API void update(rotationPlaneGrid* rotationPlaneGrid);

        vertexArrayObject* getVao() const { return _vao; };
        mappedVertexBuffer<rotationPlaneGrid*, mat4>* getModelMatricesBuffer() const { return _modelMatricesBuffer; }
        mappedBuffer<rotationPlaneGrid*, rotationPlaneGridRenderData>* getMousePlaneGridRenderDataBuffer() const { return _planeGridRenderDataBuffer; }

    private:
        void createVao();
        void createPlaneGridRenderDataBuffer();
        geometry* createPlaneQuad();
        void addPlaneGridToBuffers(rotationPlaneGrid* rotationPlaneGrid);
        void removePlaneGridFromBuffers(rotationPlaneGrid* rotationPlaneGrid);
        void assignChangedEvents(rotationPlaneGrid* rotationPlaneGrid);
        void unassignChangedEvents(rotationPlaneGrid* rotationPlaneGrid);
        void planeGridVisibleChanged(rotationPlaneGrid* rotationPlaneGrid);
        void updateModelMatrix(rotationPlaneGrid* rotationPlaneGrid);
        void updateRenderData(rotationPlaneGrid* rotationPlaneGrid);

    private:
        vertexArrayObject* _vao;
        mappedVertexBuffer<rotationPlaneGrid*, mat4>* _modelMatricesBuffer;
        mappedBuffer<rotationPlaneGrid*, rotationPlaneGridRenderData>* _planeGridRenderDataBuffer;
        unordered_map<rotationPlaneGrid*, rotationPlaneGridEventTokens> _planeGridEventTokens;
    };
}
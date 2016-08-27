#pragma once
#include <phi.h>
#include "sceneRenderingApi.h"
#include "planeGridRenderData.h"

#include <core\planeGrid.h>

#include <rendering\vertexBuffer.h>
#include <rendering\indexBuffer.h>
#include <rendering\vertexArrayObject.h>
#include <rendering\mappedVertexBuffer.h>

namespace phi
{
    class planeGridRenderAdapter
    {
        struct planeGridEventTokens
        {
            eventToken colorChangedEventToken;
            eventToken lineThicknessChangedEventToken;
            eventToken opacityChangedEventToken;
            eventToken visibleChangedEventToken;
            eventToken clippingPlanesChangedEventToken;
        };

    private:
        vertexArrayObject* _vao;
        mappedVertexBuffer<planeGrid*, mat4>* _modelMatricesBuffer;
        mappedBuffer<planeGrid*, planeGridRenderData>* _planeGridRenderDataBuffer;
        unordered_map<planeGrid*, planeGridEventTokens> _planeGridEventTokens;

    private:
        void createVao();
        void createPlaneGridRenderDataBuffer();
        geometry* createPlaneQuad();
        void addPlaneGridToBuffers(planeGrid* planeGrid);
        void removePlaneGridFromBuffers(planeGrid* planeGrid);
        void assignChangedEvents(planeGrid* planeGrid);
        void unassignChangedEvents(planeGrid* planeGrid);
        void planeGridVisibleChanged(planeGrid* planeGrid);
        void updateModelMatrix(planeGrid* planeGrid);
        void updateRenderData(planeGrid* planeGrid);

    public:
        SCENE_RENDERING_API planeGridRenderAdapter();
        SCENE_RENDERING_API ~planeGridRenderAdapter();

        SCENE_RENDERING_API void add(planeGrid* planeGrid);
        SCENE_RENDERING_API void remove(planeGrid* planeGrid);
        SCENE_RENDERING_API void update(planeGrid* planeGrid);

        vertexArrayObject* getVao() const { return _vao; };
        mappedVertexBuffer<planeGrid*, mat4>* getModelMatricesBuffer() const { return _modelMatricesBuffer; }
        mappedBuffer<planeGrid*, planeGridRenderData>* getMousePlaneGridRenderDataBuffer() const { return _planeGridRenderDataBuffer; }
    };
}
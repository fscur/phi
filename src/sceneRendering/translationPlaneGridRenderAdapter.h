#pragma once

#include <phi.h>

#include <core/translationPlaneGrid.h>

#include <rendering/indexBuffer.h>
#include <rendering/mappedVertexBuffer.h>
#include <rendering/vertexArrayObject.h>
#include <rendering/vertexBuffer.h>

#include "sceneRenderingApi.h"
#include "translationPlaneGridRenderData.h"

namespace phi
{
    class translationPlaneGridRenderAdapter
    {
        struct translationPlaneGridEventTokens
        {
            eventToken colorChangedEventToken;
            eventToken lineThicknessChangedEventToken;
            eventToken opacityChangedEventToken;
            eventToken visibleChangedEventToken;
            eventToken clippingPlanesChangedEventToken;
        };

    public:
        SCENE_RENDERING_API translationPlaneGridRenderAdapter();
        SCENE_RENDERING_API ~translationPlaneGridRenderAdapter();

        SCENE_RENDERING_API void add(translationPlaneGrid* planeGrid);
        SCENE_RENDERING_API void remove(translationPlaneGrid* planeGrid);
        SCENE_RENDERING_API void update(translationPlaneGrid* planeGrid);

        vertexArrayObject* getVao() const { return _vao; };
        mappedVertexBuffer<translationPlaneGrid*, mat4>* getModelMatricesBuffer() const { return _modelMatricesBuffer; }
        mappedBuffer<translationPlaneGrid*, translationPlaneGridRenderData>* getMousePlaneGridRenderDataBuffer() const { return _planeGridRenderDataBuffer; }

    private:
        void createVao();
        void createPlaneGridRenderDataBuffer();
        geometry* createPlaneQuad();
        void addPlaneGridToBuffers(translationPlaneGrid* planeGrid);
        void removePlaneGridFromBuffers(translationPlaneGrid* planeGrid);
        void assignChangedEvents(translationPlaneGrid* planeGrid);
        void unassignChangedEvents(translationPlaneGrid* planeGrid);
        void planeGridVisibleChanged(translationPlaneGrid* planeGrid);
        void updateModelMatrix(translationPlaneGrid* planeGrid);
        void updateRenderData(translationPlaneGrid* planeGrid);

    private:
        vertexArrayObject* _vao;
        mappedVertexBuffer<translationPlaneGrid*, mat4>* _modelMatricesBuffer;
        mappedBuffer<translationPlaneGrid*, translationPlaneGridRenderData>* _planeGridRenderDataBuffer;
        unordered_map<translationPlaneGrid*, translationPlaneGridEventTokens> _planeGridEventTokens;
    };
}
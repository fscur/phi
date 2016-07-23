#pragma once
#include <phi.h>
#include "uiRenderingApi.h"
#include "planeGridRenderData.h"

#include <ui\planeGrid.h>

#include <rendering\vertexBuffer.h>
#include <rendering\indexBuffer.h>
#include <rendering\vertexArrayObject.h>
#include <rendering\mappedVertexBuffer.h>

namespace phi
{
    class planeGridRenderAdapter
    {
    private:
        vertexArrayObject* _vao;
        mappedVertexBuffer<planeGrid*, mat4>* _modelMatricesBuffer;
        mappedBuffer<planeGrid*, planeGridRenderData>* _planeGridRenderDataBuffer;

    private:
        void createVao();
        void createPlaneGridRenderDataBuffer();
        geometry* createPlaneQuad();
        void updateModelMatrix(planeGrid* planeGrid);

    public:
        UI_RENDERING_API planeGridRenderAdapter();
        UI_RENDERING_API ~planeGridRenderAdapter();

        UI_RENDERING_API void add(planeGrid* planeGrid);
        UI_RENDERING_API void remove(planeGrid* planeGrid);
        UI_RENDERING_API void update(planeGrid* planeGrid);

        vertexArrayObject* getVao() const { return _vao; };
        mappedVertexBuffer<planeGrid*, mat4>* getModelMatricesBuffer() const { return _modelMatricesBuffer; }
        mappedBuffer<planeGrid*, planeGridRenderData>* getPlaneGridRenderDataBuffer() const { return _planeGridRenderDataBuffer; }
    };
}
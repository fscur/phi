#pragma once
#include <phi.h>
#include "uiRenderingApi.h"

#include <ui\control.h>

#include <rendering\buffer.h>

#include <rendering\mappedBuffer.h>
#include <rendering\mappedVertexBuffer.h>
#include <rendering\vertexArrayObject.h>

#include "controlRenderData.h"

namespace phi
{
    class controlRenderAdapter
    {
    private:
        const size_t MAX_CONTROL_INSTANCES = 1000;

        vertexArrayObject* _vao;
        mappedVertexBuffer<control*, mat4>* _modelMatricesBuffer;
        mappedBuffer<control*, controlRenderData>* _renderDataBuffer;

    private:
        void createBuffers();
        void updateModelMatrix(control* control);
        void updateControlRenderData(control* control);

    public:
        UI_RENDERING_API controlRenderAdapter();
        UI_RENDERING_API ~controlRenderAdapter();

        UI_RENDERING_API void add(control* control);
        UI_RENDERING_API void remove(control* control);
        UI_RENDERING_API void update(control* control);

        vertexArrayObject* getVao() const { return _vao; };
        mappedBuffer<control*, controlRenderData>* getControlRenderDataBuffer() const { return _renderDataBuffer; };
    };
}
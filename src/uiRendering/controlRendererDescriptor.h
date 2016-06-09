#pragma once
#include <phi.h>
#include "uiRenderingApi.h"

#include <ui\control.h>

#include <rendering\buffer.h>
#include <rendering\vertexBuffer.h>
#include <rendering\vertexArrayObject.h>

#include "controlRenderData.h"

namespace phi
{
    class controlRendererDescriptor
    {
    private:
        struct controlInstance
        {
            size_t bufferOffset;
            mat4 modelMatrix;
            controlRenderData renderData;
        };

    private:
        unordered_map<control*, controlInstance*> _instances;

        vector<mat4> _modelMatrices;
        vector<controlRenderData> _renderData;
        vertexBuffer* _modelMatricesBuffer;

    public:
        vertexArrayObject* _vao;
        buffer* _controlsRenderDataBuffer;

    private:
        void createBuffers();

    public:
        UI_RENDERING_API controlRendererDescriptor();
        UI_RENDERING_API ~controlRendererDescriptor();

        UI_RENDERING_API void add(control* control);
        UI_RENDERING_API void remove(control* control);
        UI_RENDERING_API void update(control* control);
    };
}
#pragma once
#include <phi.h>
#include "uiRenderingApi.h"

#include <core\resolution.h>

#include <ui\control.h>

#include <rendering\buffer.h>
#include <rendering\vertexBuffer.h>
#include <rendering\vertexArrayObject.h>
#include <rendering\renderTarget.h>

#include "controlRenderData.h"

namespace phi
{
    class glassyControlRendererDescriptor
    {
    private:
        struct glassyControlUniformBlockData
        {
            vec2 resolution;
            float backgroundPage;
            float pad0;
            int backgroundUnit;
            int level;
            int pad1;
            int pad2;
        };

        struct controlInstance
        {
            size_t bufferOffset;
            mat4 modelMatrix;
            controlRenderData renderData;
        };

    private:
        resolution _resolution;
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
        UI_RENDERING_API glassyControlRendererDescriptor(resolution resolution);
        UI_RENDERING_API ~glassyControlRendererDescriptor();

        UI_RENDERING_API void add(control* control);
        UI_RENDERING_API void remove(control* control);
        UI_RENDERING_API void update(control* control);
    };
}
#pragma once
#include <phi.h>
#include "uiRenderingApi.h"

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
        gl* _gl;
        float _width;
        float _height;
        unordered_map<control*, controlInstance*> _instances;

        vector<mat4> _modelMatrices;
        vector<controlRenderData> _renderData;
        vertexBuffer* _modelMatricesBuffer;

    public:
        vertexArrayObject* _vao;
        buffer* _controlsRenderDataBuffer;
        buffer* _uniformBlockBuffer;

    private:
        void createBuffers();

    public:
        UI_RENDERING_API glassyControlRendererDescriptor(gl* gl, float width, float height);
        UI_RENDERING_API ~glassyControlRendererDescriptor();

        UI_RENDERING_API void add(control* control);
        UI_RENDERING_API void remove(control* control);
        UI_RENDERING_API void update(control* control);
        UI_RENDERING_API void updateGlassyUniformBlock(renderTarget * renderTarget);
    };
}
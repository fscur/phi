#pragma once
#include <phi.h>
#include "uiRenderingApi.h"

#include <core\transform.h>

#include <rendering\gl.h>
#include <rendering\shader.h>
#include <rendering\buffer.h>
#include <rendering\vertexBuffer.h>
#include <rendering\program.h>

#include <ui\control.h>

#include "controlRenderData.h"

namespace phi
{
    class controlRenderer
    {
    private:
        struct controlInstance
        {
            size_t bufferOffset;
            mat4 modelMatrix;
            controlRenderData renderData;
        };

    private:
        gl* _gl;
        unordered_map<control*, controlInstance*> _instances;

        vector<mat4> _modelMatrices;
        vector<controlRenderData> _renderData;

        GLuint _vao;
        vertexBuffer<mat4>* _modelMatricesBuffer;
        buffer<controlRenderData>* _controlsRenderDataBuffer;

    private:
        void createBuffers();

    public:
        UI_RENDERING_API controlRenderer(gl* gl);
        UI_RENDERING_API ~controlRenderer();

        UI_RENDERING_API void add(control* control);
        UI_RENDERING_API void remove(control* control);
        UI_RENDERING_API void update(control* control);
        UI_RENDERING_API void render(program* shader);
    };
}
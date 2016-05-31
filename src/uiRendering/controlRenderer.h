#pragma once
#include <phi.h>
#include "renderingApi.h"

#include <core\transform.h>

#include "gl.h"
#include "shader.h"
#include "buffer.h"
#include "vertexBuffer.h"
#include "controlRenderData.h"

namespace phi
{
    class controlRenderer
    {
    private:
        gl* _gl;
        uint _controlsCount;

        vector<mat4> _modelMatrices;
        vector<controlRenderData> _renderData;

        GLuint _vao;
        vertexBuffer<mat4>* _controlsModelMatricesBuffer;
        buffer<controlRenderData>* _controlsRenderDataBuffer;

    private:
        void createBuffers();

    public:
        RENDERING_API controlRenderer(gl* gl);
        RENDERING_API ~controlRenderer();


        RENDERING_API void add(controlRenderData* renderData, transform* transform);
        RENDERING_API void render(program* shader);
    };
}
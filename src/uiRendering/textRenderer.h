#pragma once
#include <phi.h>
#include "renderingApi.h"

#include "textRenderData.h"
#include "glyphRenderData.h"
#include "vertexBuffer.h"
#include "buffer.h"
#include "shader.h"
#include "gl.h"

namespace phi
{
    class textRenderer
    {
    private:
        gl* _gl;

        uint _glyphCount;

        vector<mat4> _modelMatrices;
        vector<glyphRenderData> _renderData;

        GLuint _vao;
        vertexBuffer<mat4>* _modelMatricesBuffer;
        buffer<glyphRenderData>* _renderDataBuffer;
        vertexBuffer<uint>* _glyphIdsBuffer;
    private:
        void createBuffers();
    public:
        RENDERING_API textRenderer(gl* gl);
        RENDERING_API ~textRenderer();

        RENDERING_API void add(textRenderData* renderData);
        RENDERING_API void render(program* program);
    };
}
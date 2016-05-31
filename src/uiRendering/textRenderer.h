#pragma once
#include <phi.h>
#include "uiRenderingApi.h"

#include <rendering\vertexBuffer.h>
#include <rendering\buffer.h>
#include <rendering\shader.h>
#include <rendering\gl.h>

#include "glyphRenderData.h"
#include "textRenderData.h"

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
        UI_RENDERING_API textRenderer(gl* gl);
        UI_RENDERING_API ~textRenderer();

        UI_RENDERING_API void add(textRenderData* renderData);
        UI_RENDERING_API void render(program* program);
    };
}
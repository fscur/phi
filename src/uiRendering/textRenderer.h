#pragma once
#include <phi.h>
#include "uiRenderingApi.h"

#include <rendering\vertexBuffer.h>
#include <rendering\buffer.h>
#include <rendering\shader.h>
#include <rendering\gl.h>
#include <rendering\program.h>

#include <ui\text.h>

#include "glyphRenderData.h"
#include "textRenderData.h"

namespace phi
{
    class textRenderer
    {
    private:
        struct glyphInstance
        {
            glyphRenderData renderData;
            mat4 modelMatrix;
        };

        struct textInstance
        {
            size_t bufferOffset;
            vector<glyphInstance*> glyphs;
        };

    private:
        gl* _gl;
        size_t _glyphCount;
        map<text*, textInstance*> _instances;

        GLuint _vao;
        vertexBuffer<uint>* _glyphIdsBuffer;
        vertexBuffer<mat4>* _modelMatricesBuffer;
        buffer<glyphRenderData>* _renderDataBuffer;

    private:
        void createBuffers();
        void updateBuffers();
        textInstance* buildTextInstance(text* text);

    public:
        UI_RENDERING_API textRenderer(gl* gl);
        UI_RENDERING_API ~textRenderer();

        UI_RENDERING_API void add(text* text);
        UI_RENDERING_API void remove(text* text);
        UI_RENDERING_API void update(text* text);
        UI_RENDERING_API void render(program* program);
    };
}
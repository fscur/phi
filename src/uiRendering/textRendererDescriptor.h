#pragma once
#include <phi.h>
#include "uiRenderingApi.h"

#include <rendering\vertexBuffer.h>
#include <rendering\buffer.h>
#include <rendering\shader.h>
#include <rendering\gl.h>
#include <rendering\program.h>
#include <rendering\instancedTextVao.h>

#include <ui\text.h>

#include "glyphRenderData.h"

namespace phi
{
    class textRendererDescriptor
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
        vec2 _texelSize;
        size_t _glyphCount;
        unordered_map<text*, textInstance*> _instances;

        vertexBuffer* _modelMatricesBuffer;
    public:
        instancedTextVao* _vao;
        buffer* _glyphRenderDataBuffer;

    private:
        void createBuffers();
        void recreateBuffers();
        textInstance* buildTextInstance(text* text);

    public:
        UI_RENDERING_API textRendererDescriptor(gl* gl);
        UI_RENDERING_API ~textRendererDescriptor();

        UI_RENDERING_API void add(text* text);
        UI_RENDERING_API void remove(text* text);
        UI_RENDERING_API void update(text* text);
    };
}
#pragma once
#include <phi.h>
#include "uiRenderingApi.h"

#include <rendering\vertexBuffer.h>
#include <rendering\buffer.h>
#include <rendering\shader.h>
#include <rendering\program.h>
#include <rendering\vertexArrayObject.h>

#include <ui\text.h>

#include "glyphRenderData.h"

namespace phi
{
    class textRendererDescriptor
    {
    private:
        struct glyphInstance
        {
            glyphInstance(glyphRenderData renderData, mat4 modelMatrix) :
                renderData(renderData),
                modelMatrix(modelMatrix)
            {
            }

            glyphRenderData renderData;
            mat4 modelMatrix;
        };

        struct textInstance
        {
            textInstance(vector<glyphInstance*> glyphs) :
                glyphs(glyphs)
            {
            }

            size_t bufferOffset;
            vector<glyphInstance*> glyphs;
        };

    private:
        GLsizei _glyphCount;
        unordered_map<text*, textInstance*> _instances;

        vertexBuffer* _modelMatricesBuffer;
    public:
        vertexArrayObject* _vao;
        buffer* _glyphRenderDataBuffer;

    private:
        void createBuffers();
        void recreateBuffers();
        textInstance* buildTextInstance(text* text);

    public:
        UI_RENDERING_API textRendererDescriptor();
        UI_RENDERING_API ~textRendererDescriptor();

        UI_RENDERING_API void add(text* text);
        UI_RENDERING_API void remove(text* text);
        UI_RENDERING_API void update(text* text);
    };
}
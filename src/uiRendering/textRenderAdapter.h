#pragma once
#include <phi.h>
#include "uiRenderingApi.h"

#include <rendering\mappedVertexBuffer.h>
#include <rendering\mappedBuffer.h>
#include <rendering\shader.h>
#include <rendering\program.h>
#include <rendering\vertexArrayObject.h>

#include <ui\text.h>

#include "textInstance.h"

namespace phi
{
    class textRenderAdapter
    {

    private:
        vertexArrayObject* _vao;
        mappedVertexBuffer<text*, mat4>* _modelMatricesBuffer;
        mappedBuffer<text*, glyphRenderData>* _glyphRenderDataBuffer;

    private:
        void createBuffers();

    public:
        UI_RENDERING_API textRenderAdapter();
        UI_RENDERING_API ~textRenderAdapter();

        UI_RENDERING_API void add(text* text);
        UI_RENDERING_API void remove(text* text);
        UI_RENDERING_API void update(text* text);

        vertexArrayObject* getVao() const { return _vao; };
        mappedBuffer<text*, glyphRenderData>* getGlyphRenderDataBuffer() const { return _glyphRenderDataBuffer; };
    };
}
#pragma once
#include <phi.h>
#include "renderingApi.h"
#include <core\geometry.h>

#include "buffer.h"
#include "vertexBufferAttribute.h"
#include "vertexBuffer.h"
#include "iVertexBuffer.h"

namespace phi
{
    class vertexArrayObject
    {
    protected:
        GLuint _id;
        vector<iVertexBuffer*> _vbos;
        buffer* _ebo;

        std::function<void(void)> _onRender;

    public:
        RENDERING_API vertexArrayObject();
        RENDERING_API ~vertexArrayObject();

        RENDERING_API void add(iVertexBuffer* buffer);
        RENDERING_API void setEbo(buffer* buffer);
        RENDERING_API void render();
        RENDERING_API void setOnRender(std::function<void(void)> onRender) { _onRender = onRender; };

    public:
        RENDERING_API static vertexArrayObject* createPostProcessVao();
        RENDERING_API static vertexArrayObject* createQuadVao(geometry* quad, std::function<void(void)> renderFunction);
    };
}
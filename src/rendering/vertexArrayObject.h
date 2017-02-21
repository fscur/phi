#pragma once
#include <phi.h>
#include "renderingApi.h"
#include <core\geometry.h>

#include "iVertexBuffer.h"

namespace phi
{
    class vertexArrayObject
    {
    protected:
        GLuint _id;
        vector<iVertexBuffer*> _buffers;

        std::function<void(void)> _onRender;

    public:
        RENDERING_API vertexArrayObject();
        RENDERING_API ~vertexArrayObject();

        RENDERING_API void addBuffer(iVertexBuffer* buffer);
        RENDERING_API void render();
        void setOnRender(std::function<void(void)> onRender) { _onRender = onRender; };

    public:
        RENDERING_API static vertexArrayObject* createPostProcessVao();
        RENDERING_API static vertexArrayObject* createQuadVao(geometry* quad, std::function<void(void)> renderFunction);
        RENDERING_API static vertexArrayObject* createBoxVao(geometry* box, std::function<void(void)> renderFunction);
    };
}
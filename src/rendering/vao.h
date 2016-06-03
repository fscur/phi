#pragma once
#include <phi.h>
#include "renderingApi.h"

#include "buffer.h"
#include "vertexBuffer.h"

namespace phi
{
    class vao
    {
    protected:
        GLuint _id;
        vertexBuffer<vertex>* _vbo;
        buffer<uint>* _ebo;

    public:
        RENDERING_API vao();
        RENDERING_API ~vao();

        RENDERING_API void bind();
    };
}
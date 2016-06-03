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
        vertexBuffer* _vbo;
        buffer* _ebo;

    public:
        RENDERING_API vao();
        RENDERING_API ~vao();

        RENDERING_API virtual void bind();
        RENDERING_API virtual void unbind();
        RENDERING_API virtual void render() = 0;
    };
}
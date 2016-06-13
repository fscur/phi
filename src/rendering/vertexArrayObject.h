#pragma once
#include <phi.h>
#include "renderingApi.h"

#include "buffer.h"
#include "vertexBuffer.h"

namespace phi
{
    class vertexArrayObject
    {
    protected:
        GLuint _id;
        vector<vertexBuffer*> _vbos;
        buffer* _ebo;

        std::function<void(void)> _onRender;

    public:
        RENDERING_API vertexArrayObject();
        RENDERING_API ~vertexArrayObject();

        RENDERING_API void add(vertexBuffer* buffer);
        RENDERING_API void setEbo(buffer* buffer);
        RENDERING_API void render();
        void setOnRender(std::function<void(void)> onRender) { _onRender = onRender; };

    public:
        RENDERING_API static vertexArrayObject* createPostProcessVao();
    };
}
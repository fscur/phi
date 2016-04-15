#pragma once
#include <phi.h>
#include "renderingApi.h"
#include "gl.h"
#include "glError.h"

namespace phi
{
    namespace bufferTarget
    {
        enum bufferTarget
        {
            none = GL_NONE,
            array = GL_ARRAY_BUFFER,
            element = GL_ELEMENT_ARRAY_BUFFER,
            uniform = GL_UNIFORM_BUFFER,
            shader = GL_SHADER_STORAGE_BUFFER,
            drawIndirect = GL_DRAW_INDIRECT_BUFFER
        };
    }

    namespace bufferDataUsage
    {
        enum bufferDataUsage
        {
            streamDraw = GL_STREAM_DRAW,
            streamRead = GL_STREAM_READ,
            streamCopy = GL_STREAM_COPY,
            staticDraw = GL_STATIC_DRAW,
            staticRead = GL_STATIC_READ,
            staticCopy = GL_STATIC_COPY,
            dynamicDraw = GL_DYNAMIC_DRAW,
            dynamicRead = GL_DYNAMIC_READ,
            dynamicCopy = GL_DYNAMIC_COPY
        };
    }

    namespace bufferStorageUsage
    {
        enum bufferStorageUsage
        {
            none = GL_NONE,
            dynamic = GL_DYNAMIC_STORAGE_BIT,
            read = GL_MAP_READ_BIT,
            write = GL_MAP_WRITE_BIT,
            persistent = GL_MAP_PERSISTENT_BIT,
            coherent = GL_MAP_COHERENT_BIT,
            client = GL_CLIENT_STORAGE_BIT
        };
    }

    inline bufferStorageUsage::bufferStorageUsage operator|(bufferStorageUsage::bufferStorageUsage a, bufferStorageUsage::bufferStorageUsage b)
    {
        return static_cast<bufferStorageUsage::bufferStorageUsage>(static_cast<int>(a) | static_cast<int>(b));
    }

    class buffer
    {
    protected:
        GLuint id;
        bufferTarget::bufferTarget target;

    public:
        RENDERING_API buffer(bufferTarget::bufferTarget  target);
        RENDERING_API virtual ~buffer();

        RENDERING_API void bind();
        RENDERING_API void unbind();
        RENDERING_API void bindBufferBase(GLuint location);
        RENDERING_API void storage(GLsizeiptr size, const void* data, bufferStorageUsage::bufferStorageUsage usage);
        RENDERING_API void data(GLsizeiptr size, const void* data, bufferDataUsage::bufferDataUsage usage);
        RENDERING_API void subData(GLintptr offset, GLintptr size, void* data);
    };
}
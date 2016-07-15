#pragma once
#include <phi.h>
#include "renderingApi.h"

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
        const string _name;
        GLuint _id;
        const bufferTarget::bufferTarget _target;

    public:
        buffer(const string& name, const bufferTarget::bufferTarget target) :
            _name(name),
            _id(-1),
            _target(target)
        {
            glCreateBuffers(1, &_id);
            bind();
        }

        virtual ~buffer()
        {
            glDeleteBuffers(1, &_id);
        }

        void bind()
        {
            glBindBuffer(_target, _id);
        }

        void unbind()
        {
            glBindBuffer(_target, 0);
        }

        void bindBufferBase(GLuint location)
        {
            glBindBufferBase(_target, location, _id);
        }

        void storage(GLsizeiptr size, const void* const data, bufferStorageUsage::bufferStorageUsage usage)
        {
            glNamedBufferStorage(_id, size, data == nullptr ? NULL : data, usage);
        }

        void data(GLsizeiptr size, const void* const data, bufferDataUsage::bufferDataUsage usage)
        {
            glNamedBufferData(_id, size, data == nullptr ? NULL : data, usage);
        }

        void subData(GLintptr offset, GLintptr size, const void* const data)
        {
            glNamedBufferSubData(_id, offset, size, data);
        }

        bufferTarget::bufferTarget getTarget() const { return _target; }
        string getName() const { return _name; }
    };
}
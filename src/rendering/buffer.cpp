#include "precompiled.h"
#include "buffer.h"
#include "glError.h"

namespace phi
{
    buffer::buffer(bufferTarget::bufferTarget target) :
        id(-1),
        target(target)
    {
        glCreateBuffers(1, &id);
        bind();
    }

    buffer::~buffer()
    {
        glDeleteBuffers(1, &id);
    }

    inline void buffer::bind()
    {
        glBindBuffer(target, id); 
        glError::check();
    }

    inline void buffer::unbind()
    {
        glBindBuffer(target, 0);
        glError::check();
    }

    inline void buffer::bindBufferBase(GLuint location)
    {
        glBindBufferBase(target, location, id);
        glError::check();
    }

    inline void buffer::storage(GLsizeiptr size, void* data, bufferStorageUsage::bufferStorageUsage usage)
    {
        glNamedBufferStorage(id, size, data == nullptr ? NULL : data, usage);
        glError::check();
    }

    inline void buffer::data(GLsizeiptr size, void* data, bufferDataUsage::bufferDataUsage usage)
    {
        glNamedBufferData(id, size, data == nullptr ? NULL : data, usage);
        glError::check();
    }

    inline void buffer::subData(GLintptr offset, GLintptr size, void* data)
    {
        glNamedBufferSubData(id, offset, size, data);
        glError::check();
    }
}

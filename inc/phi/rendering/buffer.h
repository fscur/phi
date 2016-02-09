#ifndef _PHI_BUFFER_H_
#define _PHI_BUFFER_H_

#include <GL\glew.h>

struct bufferDataInfo
{
public:
    void* data;
    GLenum target;
    GLsizeiptr size;
    GLenum flags;

public:
    bufferDataInfo(
        void* data = nullptr,
        GLenum target = GL_NONE,
        GLsizeiptr size = 0,
        GLenum flags = 0) :
        data(data),
        target(target),
        size(size),
        flags(flags)
    {
    }
};

class buffer
{
private:
    GLuint _id;
    bufferDataInfo _info;

public:
    buffer()
    {
        _id = -1;
    }

    buffer(bufferDataInfo info) : 
        _info(info)
    {
        glCreateBuffers(1, &_id); 
        glBindBuffer(info.target, _id);
        glNamedBufferData(_id, info.size, info.data, info.flags);
    }

    ~buffer()
    {
        glDeleteBuffers(1, &_id);
    }

    inline void bind()
    {
        glBindBuffer(_info.target, _id);
    }
    
    inline void unbind()
    {
        glBindBuffer(_info.target, 0);
    }

    inline void bindBufferBase(GLuint location)
    {
        glBindBufferBase(_info.target, location, _id);
    }
};

#endif


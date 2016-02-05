#ifndef _PHI_ELEMENT_BUFFER_H_
#define _PHI_ELEMENT_BUFFER_H_

#include <GL\glew.h>

struct elementBufferDataInfo
{
public:
    void* data;
    GLsizeiptr size;
    GLenum flags;

public:
    elementBufferDataInfo(
        void* data = nullptr,
        GLsizeiptr size = 0,
        GLenum flags = 0) :
        data(data),
        size(size),
        flags(flags)
    {
    }
};

class elementBuffer
{
private:
    GLuint _id;
    void* _data;
public:
    elementBuffer(elementBufferDataInfo info) : 
        _data(info.data)
    {
        glCreateBuffers(1, &_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _id);
        glNamedBufferData(_id, info.size, info.data, info.flags);
    }

    ~elementBuffer()
    {
        glDeleteBuffers(1, &_id);
    }
};

#endif


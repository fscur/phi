#ifndef _PHI_VERTEX_BUFFER_H_
#define _PHI_VERTEX_BUFFER_H_

#include <phi\core\vertex.h>

#include <vector>

#include <GL\glew.h>

struct vertexBufferDataInfo
{
public:
    void* data;
    GLsizeiptr size;
    GLenum flags;

public:
    vertexBufferDataInfo(
        void* data = nullptr,
        GLsizeiptr size = 0,
        GLenum flags = 0) :
        data(data),
        size(size),
        flags(flags)
    {
    }
};

struct vertexAttrib
{
public:
    GLuint location;
    GLuint size;
    GLenum type;
    GLsizei stride;
    GLsizei offset;
    GLuint divisor;
public:

    vertexAttrib(
        GLuint location = -1,
        GLuint size = 0,
        GLenum type = GL_NONE,
        GLsizei stride = 0,
        GLsizei offset = 0,
        GLuint divisor = 0) :
        location(location),
        size(size),
        type(type),
        stride(stride),
        offset(offset),
        divisor(divisor)
    {
    }
};

class vertexBuffer
{
private:
    GLuint _id;
    void* _data;
public:
    vertexBuffer(vertexBufferDataInfo info, std::vector<vertexAttrib> attribs) :
        _data(info.data)
    {
        glCreateBuffers(1, &_id);
        glBindBuffer(GL_ARRAY_BUFFER, _id);
        glNamedBufferData(_id, info.size, info.data, info.flags);

        auto s = attribs.size();

        for (GLuint i = 0; i < s; i++)
        {
            auto location = attribs[i].location;
            glEnableVertexAttribArray(location);

            if (attribs[i].type == GL_UNSIGNED_INT || attribs[i].type == GL_INT)
                glVertexAttribIPointer(location, attribs[i].size, attribs[i].type, attribs[i].stride, (void*)attribs[i].offset);
            else if (attribs[i].type == GL_FLOAT)
                glVertexAttribPointer(location, attribs[i].size, attribs[i].type, GL_FALSE, attribs[i].stride, (void*)attribs[i].offset);

            glVertexAttribDivisor(location, attribs[i].divisor);
        }
    }

    ~vertexBuffer()
    {
        glDeleteBuffers(1, &_id);
    }
};

#endif


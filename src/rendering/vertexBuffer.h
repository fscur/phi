#pragma once
#include <phi.h>
#include "buffer.h"
#include <core\vertex.h>

namespace phi
{
    struct vertexAttrib
    {
    public:
        GLuint location;
        GLuint size;
        GLenum type;
        GLsizei stride;
        const void* offset;
        GLuint divisor;
    public:

        vertexAttrib(
            GLuint location = -1,
            GLuint size = 0,
            GLenum type = GL_NONE,
            GLsizei stride = 0,
            const void* offset = (const void*)0,
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

    class vertexBuffer :
        public buffer
    {
    public:
        vertexBuffer(vector<vertexAttrib> attribs) : buffer(bufferTarget::array)
        {
            auto s = attribs.size();

            for (GLuint i = 0; i < s; i++)
            {
                auto location = attribs[i].location;
                glEnableVertexAttribArray(location);

                if (attribs[i].type == GL_UNSIGNED_INT || attribs[i].type == GL_INT)
                    glVertexAttribIPointer(location, attribs[i].size, attribs[i].type, attribs[i].stride, attribs[i].offset);
                else if (attribs[i].type == GL_FLOAT)
                    glVertexAttribPointer(location, attribs[i].size, attribs[i].type, GL_FALSE, attribs[i].stride, attribs[i].offset);

                glVertexAttribDivisor(location, attribs[i].divisor);
            }
        }

        ~vertexBuffer(){}
    };
}
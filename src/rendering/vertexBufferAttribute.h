#pragma once
#include <phi.h>

namespace phi
{
    struct vertexBufferAttribute
    {
    public:
        GLuint location;
        GLuint size;
        GLenum type;
        GLsizei stride;
        const void* offset;
        GLuint divisor;
    public:

        vertexBufferAttribute(
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
}

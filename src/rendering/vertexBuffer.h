#pragma once
#include <phi.h>
#include <core\vertex.h>
#include "buffer.h"

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
    private:
        vector<vertexAttrib> _attribs;

    public:
        vertexBuffer(const string& name, const vector<vertexAttrib>& attribs) :
            buffer(name, bufferTarget::array),
            _attribs(attribs)
        {
        }

        void initialize()
        {
            glBindBuffer(_target, _id);
            auto attribsCount = _attribs.size();

            for (GLuint i = 0; i < attribsCount; i++)
            {
                auto location = _attribs[i].location;

                glEnableVertexAttribArray(location);
                glError::check();

                if (_attribs[i].type == GL_UNSIGNED_INT || _attribs[i].type == GL_INT)
                {
                    glVertexAttribIPointer(location, _attribs[i].size, _attribs[i].type, _attribs[i].stride, _attribs[i].offset);
                    glError::check();
                }
                else if (_attribs[i].type == GL_FLOAT)
                {
                    glVertexAttribPointer(location, _attribs[i].size, _attribs[i].type, GL_FALSE, _attribs[i].stride, _attribs[i].offset);
                    glError::check();
                }

                glVertexAttribDivisor(location, _attribs[i].divisor);
                glError::check();
            }
        }
    };
}
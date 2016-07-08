#pragma once
#include <phi.h>

#include "mappedBuffer.h"
#include "iVertexBuffer.h"
#include "vertexBufferAttribute.h"

namespace phi
{
    template <typename KEY, typename DATA>
    class mappedVertexBuffer :
        public mappedBuffer<KEY, DATA>,
        public iVertexBuffer
    {
    private:
        vector<vertexBufferAttribute> _attribs;

    public:
        mappedVertexBuffer(const string& name, const vector<vertexBufferAttribute>& attribs) :
            mappedBuffer<KEY, DATA>(name, bufferTarget::array),
            _attribs(attribs)
        {
        }

        void initialize() override
        {
            glBindBuffer(_target, _id);
            auto attribsCount = _attribs.size();

            for (GLuint i = 0; i < attribsCount; i++)
            {
                auto location = _attribs[i].location;

                glEnableVertexAttribArray(location);

                if (_attribs[i].type == GL_UNSIGNED_INT || _attribs[i].type == GL_INT)
                {
                    glVertexAttribIPointer(location, _attribs[i].size, _attribs[i].type, _attribs[i].stride, _attribs[i].offset);
                }
                else if (_attribs[i].type == GL_FLOAT)
                {
                    glVertexAttribPointer(location, _attribs[i].size, _attribs[i].type, GL_FALSE, _attribs[i].stride, _attribs[i].offset);
                }

                glVertexAttribDivisor(location, _attribs[i].divisor);
            }
        }
    };
}

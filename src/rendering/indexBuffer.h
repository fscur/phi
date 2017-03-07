#pragma once
#include <phi.h>
#include <core\vertex.h>

#include "buffer.h"
#include "iVertexBuffer.h"

namespace phi
{
    class indexBuffer :
        public buffer,
        public iVertexBuffer
    {
    public:
        indexBuffer(const string& name) :
            buffer(name, bufferTarget::element)
        {
        }

        ~indexBuffer() {}

        void initialize() override
        {
            buffer::bind();
        }
    };
}
#pragma once
#include <phi.h>

#include "mappedBuffer.h"
#include "iVertexBuffer.h"

namespace phi
{
    template <typename KEY, typename DATA>
    class mappedIndexBuffer :
        public mappedBuffer<KEY, DATA>,
        public iVertexBuffer
    {
    public:
        mappedIndexBuffer(const string& name) :
            mappedBuffer<KEY, DATA>(name, bufferTarget::element)
        {
        }

        ~mappedIndexBuffer() {}

        void initialize() override
        {
            buffer::bind();
        }
    };
}

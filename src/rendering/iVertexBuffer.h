#pragma once
#include <phi.h>

namespace phi
{
    class iVertexBuffer
    {
    public:
        virtual void initialize() = 0;
        virtual ~iVertexBuffer() {}
    };
}

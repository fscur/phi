#pragma once
#include <phi.h>
#include "buffer.h"
#include "iVertexBuffer.h"

namespace phi
{
    class indexBuffer :
        public buffer, public iVertexBuffer
    {
    private:


    public:
        indexBuffer();
        ~indexBuffer();
    };
}
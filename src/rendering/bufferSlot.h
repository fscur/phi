#pragma once
#include <phi.h>

namespace phi
{
    struct bufferSlot
    {
        size_t index;
        size_t size;
        bufferSlot(size_t index = 0u, size_t size = 1u) :
            index(index),
            size(size)
        {
        }
    };
}

#pragma once
#include <phi.h>

namespace phi
{
    struct deallocation
    {
        deallocation(void* address) :
            address(address)
        {
        }

        void* address;

        void print(action<char*> print)
        {
            char buffer[1024];
            sprintf_s(buffer, "deallocation at address (%p)", address);

            print(buffer);
        }
    };
}
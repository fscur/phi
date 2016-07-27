#pragma once
#include <phi.h>

namespace phi
{
    struct allocation
    {
        allocation(void* address) :
            address(address)
        {
        }

        void* address;

        void print(action<char*> print)
        {
            char buffer[1024];
            sprintf_s(buffer, "memory leak at address (%p)", address);

            print(buffer);
        }
    };
}
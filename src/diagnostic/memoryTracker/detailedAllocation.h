#pragma once
#include <phi.h>

#include <diagnostic\stackTracer.h>
#include <diagnostic\stackSymbol.h>

#include "allocation.h"

namespace phi
{
    struct detailedAllocation :
        public allocation
    {
    private:
        const uint8_t FRAMES_TO_SKIP_WHEN_CAPTURING_ALLOCATION = 8;
    public:
        detailedAllocation(void* address) :
            allocation::allocation(address),
            site(stackTracer::optimizedCaptureAllocationSight(FRAMES_TO_SKIP_WHEN_CAPTURING_ALLOCATION))
        {
        }

        stackSymbol site;

        void print(action<char*> print)
        {
            char buffer[1024];

            sprintf_s(buffer,
                "%s(%d): memory leak error: allocated (%p) at %s\n",
                site.file.name,
                site.file.line,
                address,
                site.name);

            print(buffer);
        }
    };
}
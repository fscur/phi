#pragma once
#include <phi.h>

#include <diagnostic\stackTracer.h>
#include <diagnostic\stackSymbol.h>

#include "deallocation.h"

namespace phi
{
    struct detailedDeallocation :
        public deallocation
    {
    private:
        const uint8_t FRAMES_TO_SKIP_WHEN_CAPTURING_DEALLOCATION = 10;

    public:
        detailedDeallocation(void* address) :
            deallocation::deallocation(address),
            site(stackTracer::optimizedCaptureAllocationSight(FRAMES_TO_SKIP_WHEN_CAPTURING_DEALLOCATION))
        {
        }

        stackSymbol site;

        void print(action<char*> print)
        {
            char messageBuffer[1024];

            sprintf_s(messageBuffer,
                "%s(%d): unnecessary deallocation error: at %s\n",
                site.file.name,
                site.file.line,
                site.name);

            print(messageBuffer);
        }
    };
}
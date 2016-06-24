#pragma once
#include <phi.h>

#include <rendering\renderPass.h>
#include <rendering\framebufferAllocator.h>

#include "controlRenderAdapter.h"

namespace phi
{
    class controlRenderPass
    {
    public:
        controlRenderPass() = delete;
        static renderPass* configure(
            controlRenderAdapter* renderAdapter, 
            const resolution& resolution,
            const string& shadersPath,
            framebufferAllocator* framebufferAllocator);
    };
}
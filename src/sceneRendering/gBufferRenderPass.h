#pragma once
#include <phi.h>

#include <core\resolution.h>

#include <rendering\renderPass.h>
#include <rendering\framebufferAllocator.h>

#include "meshRenderAdapter.h"

namespace phi
{
    class gBufferRenderPass
    {
    public:
        gBufferRenderPass() = delete;
        static renderPass* configure(
            meshRenderAdapter* meshAdapter,
            const resolution& resolution,
            const string& shadersPath,
            framebufferAllocator* framebufferAllocator);
    };
}
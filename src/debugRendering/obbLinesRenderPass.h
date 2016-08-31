#pragma once
#include <phi.h>
#include "debugRenderingApi.h"
#include <core\resolution.h>
#include <rendering\renderPass.h>
#include <rendering\framebufferAllocator.h>

#include "obbRenderAdapter.h"

namespace phi
{
    class obbLinesRenderPass
    {
    public:
        obbLinesRenderPass() = delete;
        DEBUG_RENDERING_API static renderPass* configure(
            const obbRenderAdapter* renderAdapter,
            const resolution& resolution, 
            const string& shadersPath,
            framebufferAllocator* framebufferAllocator);
    };
}

#pragma once
#include <phi.h>
#include "debugRenderingApi.h"
#include <core\resolution.h>
#include <rendering\renderPass.h>
#include <rendering\framebufferAllocator.h>

#include "boxColliderRenderAdapter.h"

namespace phi
{
    class boxColliderRenderPass
    {
    public:
        boxColliderRenderPass() = delete;
        DEBUG_RENDERING_API static renderPass* configure(
            const boxColliderRenderAdapter* renderAdapter,
            const resolution& resolution,
            const string& shadersPath,
            framebufferAllocator* framebufferAllocator);
    };
}

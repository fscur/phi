#pragma once
#include <phi.h>
#include "sceneRenderingApi.h"
#include <core\resolution.h>
#include <rendering\renderPass.h>
#include <rendering\framebufferAllocator.h>

#include "planeGridRenderAdapter.h"

namespace phi
{
    class planeGridRenderPass
    {
    public:
        planeGridRenderPass() = delete;
        SCENE_RENDERING_API static renderPass* configure(
            const planeGridRenderAdapter* renderAdapter,
            const resolution& resolution,
            const string& shadersPath,
            framebufferAllocator* framebufferAllocator);
    };
}

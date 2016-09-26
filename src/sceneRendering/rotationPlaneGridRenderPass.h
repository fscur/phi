#pragma once
#include <phi.h>
#include "sceneRenderingApi.h"
#include <core\resolution.h>
#include <rendering\renderPass.h>
#include <rendering\framebufferAllocator.h>

#include "rotationPlaneGridRenderAdapter.h"

namespace phi
{
    class rotationPlaneGridRenderPass
    {
    public:
        rotationPlaneGridRenderPass() = delete;
        SCENE_RENDERING_API static renderPass* configure(
            const rotationPlaneGridRenderAdapter* renderAdapter,
            const resolution& resolution,
            const string& shadersPath,
            framebufferAllocator* framebufferAllocator);
    };
}

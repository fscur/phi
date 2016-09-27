#pragma once
#include <phi.h>
#include "../sceneRenderingApi.h"
#include <core\resolution.h>
#include <rendering\renderPass.h>
#include <rendering\framebufferAllocator.h>

#include "skyBoxRenderAdapter.h"

namespace phi
{
    class skyBoxRenderPass
    {
    public:
        skyBoxRenderPass() = delete;
        SCENE_RENDERING_API static renderPass* configure(
            const skyBoxRenderAdapter* renderAdapter,
            const resolution& resolution,
            const string& shadersPath,
            framebufferAllocator* framebufferAllocator);
    };
}

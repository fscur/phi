#pragma once
#include <phi.h>
#include "sceneRenderingApi.h"

#include <core\resolution.h>

#include <rendering\renderPass.h>
#include <rendering\framebufferAllocator.h>

#include "meshRenderAdapter.h"

namespace phi
{
    class meshRenderer
    {
    public:
        meshRenderer() = delete;
        ~meshRenderer() = delete;

        static SCENE_RENDERING_API vector<renderPass*> configure(
            const resolution& resolution,
            const string& resourcesPath,
            meshRenderAdapter* meshAdapter,
            framebufferAllocator* framebufferAllocator);
    };
}
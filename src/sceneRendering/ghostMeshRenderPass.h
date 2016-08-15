#pragma once
#include <phi.h>
#include "sceneRenderingApi.h"


#include <core\resolution.h>

#include <rendering\renderPass.h>
#include <rendering\framebufferAllocator.h>

#include "ghostMeshRenderAdapter.h"

namespace phi
{
    class ghostMeshRenderPass
    {
    private:
        static void ghostMeshRenderPass::createRTsBuffer(
            buffer* rtsBuffer,
            const texture* rt0Texture,
            const texture* rt1Texture,
            const texture* rt2Texture,
            const texture* rtDepthTexture,
            const texture* rtPickingTexture);

    public:
        ghostMeshRenderPass() = delete;
        SCENE_RENDERING_API static renderPass* configure(
            ghostMeshRenderAdapter* meshAdapter,
            const resolution& resolution,
            const string& shadersPath,
            framebufferAllocator* framebufferAllocator);
    };
}
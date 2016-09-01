#pragma once
#include <phi.h>

#include <core\resolution.h>

#include <rendering\renderPass.h>
#include <rendering\framebufferAllocator.h>

namespace phi
{
    class lightingRenderPass
    {
    private:
        static void updateGBufferRTsBuffer(
            buffer* rtsBuffer,
            const texture* rt0Texture,
            const texture* rt1Texture,
            const texture* rt2Texture,
            const texture* rtDepthTexture,
            const texture* rtPickingTexture);
        
    public:
        lightingRenderPass() = delete;
        static renderPass* configure(
            renderPass* gBufferRenderPass,
            const resolution& resolution,
            const string& shadersPath,
            framebufferAllocator* framebufferAllocator);
    };
}

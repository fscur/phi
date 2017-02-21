#pragma once
#include <phi.h>

#include <core/resolution.h>

#include <rendering/framebufferAllocator.h>
#include <rendering/renderPass.h>

#include "ghostMeshRenderAdapter.h"
#include "sceneRenderingApi.h"

namespace phi
{
    class ghostMeshRenderPass :
        public renderPass
    {
    public:
        ghostMeshRenderPass(
            ghostMeshRenderAdapter* renderAdapter,
            const resolution& resolution,
            const string& shadersPath,
            framebufferAllocator* framebufferAllocator);

        ~ghostMeshRenderPass();
        void onInitialize() override;
        void onBeginRender() override;
        void onEndRender() override;

    private:
        static void ghostMeshRenderPass::createRTsBuffer(
            buffer* rtsBuffer,
            const texture* rt0Texture,
            const texture* rt1Texture,
            const texture* rt2Texture,
            const texture* rtDepthTexture,
            const texture* rtPickingTexture);

    private:
        renderTarget* _defaultRenderTarget;
        renderTarget* _rt0;
        renderTarget* _rt1;
        renderTarget* _rt2;
        renderTarget* _rtDepth;
        renderTarget* _rtPicking;
        buffer* _rtsBuffer;
    };
}
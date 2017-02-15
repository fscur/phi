#pragma once

#include <phi.h>

#include <core/resolution.h>
#include <rendering/framebufferAllocator.h>
#include <rendering/renderPass.h>

namespace phi
{
    class lightingRenderPass :
        public renderPass
    {
    public:
        lightingRenderPass(
            const resolution& resolution,
            const string& shadersPath,
            framebufferAllocator* framebufferAllocator);

        ~lightingRenderPass();

        void onInitialize() override;
        void onBeginRender() override;
        void onEndRender() override;

        void onResize(const resolution& resolution) override;

    private:
        static void updateGBufferRTsBuffer(
            buffer* rtsBuffer,
            const texture* rt0Texture,
            const texture* rt1Texture,
            const texture* rt2Texture,
            const texture* rtDepthTexture,
            const texture* rtPickingTexture);

    private:
        framebufferAllocator* _framebufferAllocator;
        const string _shadersPath;

        renderTarget* _rt0;
        renderTarget* _rt1;
        renderTarget* _rt2;
        renderTarget* _rtDepth;
        renderTarget* _rtPicking;

        vertexArrayObject* _quadVao;

        buffer* _rtsBuffer;
    };
}

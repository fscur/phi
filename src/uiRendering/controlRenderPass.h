#pragma once
#include <phi.h>

#include <rendering\renderPass.h>
#include <rendering\framebufferAllocator.h>

#include "controlRenderAdapter.h"

namespace phi
{
    class controlRenderPass :
        public renderPass
    {
    public:
        struct glassyControlUniformBlockData
        {
            vec2 resolution;
            float backgroundPage;
            float pad0;
            int backgroundUnit;
            int level;
            int pad1;
            int pad2;
        };
    public:
        controlRenderPass(
            controlRenderAdapter* renderAdapter,
            const resolution& resolution,
            const string& shadersPath,
            framebufferAllocator* framebufferAllocator);

        ~controlRenderPass();

        void onInitialize() override;
        void onBeginRender() override;
        void onEndRender() override;
        void onResize(const resolution& resolution) override;

    private:
        static void updateGlassyUniformBlock(buffer* buffer, renderTarget* finalImageRenderTarget, const resolution& resolution);

    private:
        buffer* _glassyUniformBlockBuffer;
        renderTarget* _defaultRenderTarget;
    };
}
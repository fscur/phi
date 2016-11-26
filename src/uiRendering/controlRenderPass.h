#pragma once
#include <phi.h>

#include <rendering\renderPass.h>
#include <rendering\framebufferAllocator.h>

#include "controlRenderAdapter.h"

namespace phi
{
    class controlRenderPass
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
        controlRenderPass() = delete;
        static renderPass* configure(
            controlRenderAdapter* renderAdapter, 
            const resolution& resolution,
            const string& shadersPath,
            framebufferAllocator* framebufferAllocator);

    private:
        static void updateGlassyUniformBlock(buffer* buffer, renderTarget* finalImageRenderTarget, const resolution& resolution);
    };
}
#pragma once
#include <phi.h>

#include <rendering\renderPass.h>
#include <rendering\framebufferAllocator.h>

#include "glassyControlRendererDescriptor.h"

namespace phi
{
    class glassyControlRenderPass
    {
    private:
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

    private:
        static void updateUniformBlock(buffer* buffer, renderTarget* finalImageRenderTarget, const resolution& resolution);

    public:
        glassyControlRenderPass() = delete;
        static renderPass* configure(
            glassyControlRendererDescriptor* rendererDescriptor, 
            const resolution& resolution,
            const string& shadersPath,
            framebufferAllocator* framebufferAllocator);
    };
}
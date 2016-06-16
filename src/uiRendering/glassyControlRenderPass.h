#pragma once
#include <phi.h>

#include <rendering\renderPass.h>
#include <rendering\framebufferAllocator.h>

#include "glassyControlRendererDescriptor.h"

namespace phi
{
    class glassyControlRenderPass
    {
    public:
        glassyControlRenderPass() = delete;
        static renderPass* configure(
            glassyControlRendererDescriptor* rendererDescriptor, 
            const resolution& resolution,
            const string& shadersPath,
            framebufferAllocator* framebufferAllocator);
    };
}
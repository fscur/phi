#pragma once
#include <phi.h>

#include <rendering\renderPass.h>

#include "controlRendererDescriptor.h"
#include <rendering\framebufferAllocator.h>

namespace phi
{
    class controlRenderPass
    {
    public:
        controlRenderPass() = delete;
        static renderPass* configure(
            controlRendererDescriptor* rendererDescriptor, 
            const resolution& resolution,
            const string& shadersPath,
            framebufferAllocator* framebufferAllocator);
    };
}
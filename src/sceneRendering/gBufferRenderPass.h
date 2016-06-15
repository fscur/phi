#pragma once
#include <phi.h>

#include <core\resolution.h>

#include <rendering\renderPass.h>

#include "meshRendererDescriptor.h"
#include <rendering\framebufferAllocator.h>

namespace phi
{
    class gBufferRenderPass
    {
    public:
        gBufferRenderPass() = delete;
        static renderPass* configure(
            meshRendererDescriptor* rendererDescriptor,
            const resolution& resolution,
            const string& shadersPath,
            framebufferAllocator* framebufferAllocator);
    };
}
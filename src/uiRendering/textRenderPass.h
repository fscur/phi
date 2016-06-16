#pragma once
#include <phi.h>

#include <core\resolution.h>

#include <rendering\renderPass.h>

#include "textRendererDescriptor.h"
#include <rendering\framebufferAllocator.h>

namespace phi
{
    class textRenderPass
    {
    public:
        textRenderPass() = delete;
        static renderPass * configure(
            textRendererDescriptor* rendererDescriptor, 
            const resolution& resolution,
            const string& shadersPath,
            framebufferAllocator* framebufferAllocator);
    };
}
#pragma once
#include <phi.h>

#include <core\resolution.h>

#include "renderPass.h"
#include "framebufferAllocator.h"

namespace phi
{
    class verticalGaussianBlurRenderPass
    {
    public:
        verticalGaussianBlurRenderPass() = delete;
        ~verticalGaussianBlurRenderPass() = delete;

        RENDERING_API static renderPass* configure(
            const resolution& resolution,
            const string & shadersPath,
            framebufferAllocator * framebufferAllocator);
    };
}
#pragma once
#include <phi.h>
#include "renderingApi.h"

#include "renderTarget.h"
#include "frameBuffer.h"
#include "framebufferLayoutBuilder.h"
#include "framebufferAllocator.h"

namespace phi
{
    class pickingFramebuffer
    {
    private:
        static framebuffer* _pickingFramebuffer;
        static renderTarget* _pickingRenderTarget;
        static resolution _resolution;

    public:
        pickingFramebuffer() = delete;
        ~pickingFramebuffer() = delete;

        RENDERING_API static void initialize(framebufferAllocator* framebufferAllocator, const resolution& resolution);
        RENDERING_API static uint32_t pick(int mouseX, int mouseY);
        RENDERING_API static void resize(const resolution& resolution);
    };
}
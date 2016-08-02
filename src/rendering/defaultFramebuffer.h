#pragma once
#include <phi.h>
#include "renderingApi.h"

#include "renderTarget.h"
#include "frameBuffer.h"
#include "framebufferLayoutBuilder.h"
#include "framebufferAllocator.h"

namespace phi
{
    class defaultFramebuffer
    {
    private:
        static framebuffer* _defaultFramebuffer;
        static resolution _resolution;

    public:
        defaultFramebuffer() = delete;
        ~defaultFramebuffer() = delete;

        RENDERING_API static void initialize(framebufferAllocator* framebufferAllocator, const resolution& resolution);
        RENDERING_API static void resize(const resolution& resolution);
        RENDERING_API static GLfloat getZBufferValue(int x, int y);
    };
}
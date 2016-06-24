#include <precompiled.h>
#include "pickingFramebuffer.h"

namespace phi
{
    framebuffer* pickingFramebuffer::_pickingFramebuffer = nullptr;
    renderTarget* pickingFramebuffer::_pickingRenderTarget = nullptr;
    resolution pickingFramebuffer::_resolution;

    void pickingFramebuffer::initialize(framebufferAllocator* framebufferAllocator, const resolution& resolution)
    {
        auto pickingFramebufferLayout = framebufferLayoutBuilder::newFramebufferLayout("pickingFramebuffer")
            .with("pickingRenderTarget", GL_COLOR_ATTACHMENT0, GL_RGBA8, GL_RGBA)
            .build();

        _pickingFramebuffer = framebufferAllocator->newFramebuffer(pickingFramebufferLayout, resolution);
        _pickingRenderTarget = _pickingFramebuffer->getRenderTarget("pickingRenderTarget");
        _resolution = resolution;
    }

    uint32_t pickingFramebuffer::pick(int mouseX, int mouseY)
    {
        auto x = static_cast<GLint>(mouseX);
        auto y = static_cast<GLint>(_resolution.height - mouseY);

        auto pixels = _pickingFramebuffer->readPixels(_pickingRenderTarget, x, y, 1, 1);

        auto r = static_cast<int>(pixels.r);
        auto g = static_cast<int>(pixels.g) << 8;
        auto b = static_cast<int>(pixels.b) << 16;

        return r | g | b;
    }

    void pickingFramebuffer::resize(const resolution& resolution)
    {
        _resolution = resolution;
    }
}
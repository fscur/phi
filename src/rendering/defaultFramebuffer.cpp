#include <precompiled.h>
#include "defaultFramebuffer.h"
#include "texturesManager.h"

namespace phi
{
    framebuffer* defaultFramebuffer::_defaultFramebuffer = nullptr;
    resolution defaultFramebuffer::_resolution;

    void defaultFramebuffer::initialize(framebufferAllocator* framebufferAllocator, const resolution& resolution)
    {
        _resolution = resolution;

        auto w = static_cast<uint>(resolution.width);
        auto h = static_cast<uint>(resolution.height);

        auto renderTargetLayout = textureLayout();
        renderTargetLayout.dataFormat = GL_RGB;
        renderTargetLayout.dataType = GL_UNSIGNED_BYTE;
        renderTargetLayout.internalFormat = GL_RGB8;
        renderTargetLayout.wrapMode = GL_CLAMP_TO_EDGE;
        renderTargetLayout.minFilter = GL_LINEAR_MIPMAP_LINEAR;
        renderTargetLayout.magFilter = GL_LINEAR;
        renderTargetLayout.levels = texturesManager::getMaxLevels(w, h);

        auto renderTargetTexture = new texture(
            w,
            h,
            renderTargetLayout,
            nullptr);

        auto defaultRenderTarget = new renderTarget("defaultRenderTarget", renderTargetTexture);

        auto depthRenderTargetLayout = textureLayout();
        depthRenderTargetLayout.dataFormat = GL_DEPTH_COMPONENT;
        depthRenderTargetLayout.dataType = GL_FLOAT;
        depthRenderTargetLayout.internalFormat = GL_DEPTH_COMPONENT32;
        depthRenderTargetLayout.wrapMode = GL_CLAMP_TO_EDGE;
        depthRenderTargetLayout.minFilter = GL_NEAREST;
        depthRenderTargetLayout.magFilter = GL_NEAREST;

        auto depthRenderTargetTexture = new texture(
            w,
            h,
            depthRenderTargetLayout,
            nullptr);

        auto depthRenderTarget = new renderTarget("depthRenderTarget", depthRenderTargetTexture);

        auto defaultFramebufferLayout = framebufferLayoutBuilder::newFramebufferLayout("defaultFramebuffer")
            .with(defaultRenderTarget, GL_COLOR_ATTACHMENT0)
            .with(depthRenderTarget, GL_DEPTH_ATTACHMENT)
            .build();

        _defaultFramebuffer = framebufferAllocator->newFramebuffer(defaultFramebufferLayout, resolution);
        safeDelete(defaultFramebufferLayout);
    }

    void defaultFramebuffer::resize(const resolution& resolution)
    {
        _resolution = resolution;
    }

    GLfloat defaultFramebuffer::getZBufferValue(int x, int y)
    {
        return _defaultFramebuffer->getZBufferValue(x, y);
    }
}
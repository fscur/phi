#include <precompiled.h>
#include "controlRenderPass.h"

#include <rendering\textureUnits.h>
#include <rendering\texturesManager.h>
#include <rendering\vertexArrayObject.h>
#include <rendering\programBuilder.h>
#include <rendering\framebufferLayoutBuilder.h>

namespace phi
{
    controlRenderPass::controlRenderPass(
        controlRenderAdapter* renderAdapter, 
        const resolution& resolution, 
        const string& shadersPath, 
        framebufferAllocator* framebufferAllocator) :
        renderPass(resolution)
    {
        _glassyUniformBlockBuffer = new buffer("GlassyControlUniformBlock", bufferTarget::uniform);
        _glassyUniformBlockBuffer->data(
            sizeof(glassyControlUniformBlockData),
            nullptr,
            bufferDataUsage::dynamicDraw);

        _program = programBuilder::buildProgram(shadersPath, "control", "control");
        _program->addBuffer(renderAdapter->getControlRenderDataBuffer());
        _program->addBuffer(_glassyUniformBlockBuffer);

        auto defaultFramebuffer = framebufferAllocator->getFramebuffer("defaultFramebuffer");
        _defaultRenderTarget = defaultFramebuffer->getRenderTarget("defaultRenderTarget");

        auto pickingRenderTarget = framebufferAllocator->getRenderTarget("pickingRenderTarget");

        auto depthRenderTargetLayout = textureLayout();
        depthRenderTargetLayout.dataFormat = GL_DEPTH_COMPONENT;
        depthRenderTargetLayout.dataType = GL_FLOAT;
        depthRenderTargetLayout.internalFormat = GL_DEPTH_COMPONENT32;
        depthRenderTargetLayout.wrapMode = GL_CLAMP_TO_EDGE;
        depthRenderTargetLayout.minFilter = GL_NEAREST;
        depthRenderTargetLayout.magFilter = GL_NEAREST;

        auto depthRenderTargetTexture = new texture(
            static_cast<uint>(resolution.width),
            static_cast<uint>(resolution.height),
            depthRenderTargetLayout,
            nullptr);

        auto depthRenderTarget = new renderTarget("controlDepthRenderTarget", depthRenderTargetTexture);

        auto controlFramebufferLayout = framebufferLayoutBuilder::newFramebufferLayout("controlFrameBuffer")
            .with(_defaultRenderTarget, GL_COLOR_ATTACHMENT0)
            .with(pickingRenderTarget, GL_COLOR_ATTACHMENT1)
            .with(depthRenderTarget, GL_DEPTH_ATTACHMENT)
            .build();

        _framebuffer = framebufferAllocator->newFramebuffer(controlFramebufferLayout, resolution);
        safeDelete(controlFramebufferLayout);

        addVao(renderAdapter->getVao());
    }

    controlRenderPass::~controlRenderPass()
    {
        safeDelete(_glassyUniformBlockBuffer);
    }
    
    void controlRenderPass::onInitialize()
    {
        updateGlassyUniformBlock(_glassyUniformBlockBuffer, _defaultRenderTarget, _resolution);
    }

    void controlRenderPass::onBeginRender()
    {
        _framebuffer->bindForDrawing();

        auto one = 1.0f;
        glClearBufferfv(GL_DEPTH, 0, &one);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendColor(1, 1, 1, 1);

        _program->bind();

        if (texturesManager::getIsBindless())
            _program->setUniform(0, texturesManager::textureArraysHandles);
        else
            _program->setUniform(0, texturesManager::textureArraysUnits);
    }

    void controlRenderPass::onEndRender()
    {
        _program->unbind();
        _framebuffer->blitToDefault(_defaultRenderTarget, 0, 0, _defaultRenderTarget->texture->w, _defaultRenderTarget->texture->h);

        glBlendColor(0, 0, 0, 0);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
    }

    void controlRenderPass::onResize(const resolution & resolution)
    {
        updateGlassyUniformBlock(_glassyUniformBlockBuffer, _defaultRenderTarget, resolution);
    }

    void controlRenderPass::updateGlassyUniformBlock(buffer* buffer, renderTarget* finalImageRenderTarget, const resolution& resolution)
    {
        auto uniformBlockData = glassyControlUniformBlockData();
        auto rtAddress = texturesManager::getTextureAddress(finalImageRenderTarget->texture);
        uniformBlockData.backgroundPage = rtAddress.page;
        uniformBlockData.backgroundUnit = rtAddress.unit;
        uniformBlockData.level = 2;
        uniformBlockData.resolution = resolution.toVec2();

        buffer->data(
            sizeof(glassyControlUniformBlockData),
            &uniformBlockData,
            bufferDataUsage::dynamicDraw);
    }
}
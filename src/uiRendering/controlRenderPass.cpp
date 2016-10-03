#include <precompiled.h>
#include "controlRenderPass.h"

#include <rendering\textureUnits.h>
#include <rendering\texturesManager.h>
#include <rendering\vertexArrayObject.h>
#include <rendering\programBuilder.h>
#include <rendering\framebufferLayoutBuilder.h>

namespace phi
{
    renderPass* controlRenderPass::configure(
        controlRenderAdapter* renderAdapter,
        const resolution& resolution,
        const string& shadersPath,
        framebufferAllocator* framebufferAllocator)
    {
        auto glassyUniformBlockBuffer = new buffer("GlassyControlUniformBlock", bufferTarget::uniform);
        glassyUniformBlockBuffer->data(
            sizeof(glassyControlUniformBlockData),
            nullptr,
            bufferDataUsage::dynamicDraw);

        auto program = programBuilder::buildProgram(shadersPath, "control", "control");
        program->addBuffer(renderAdapter->getControlRenderDataBuffer());
        program->addBuffer(glassyUniformBlockBuffer);

        auto defaultFramebuffer = framebufferAllocator->getFramebuffer("defaultFramebuffer");
        auto defaultRenderTarget = defaultFramebuffer->getRenderTarget("defaultRenderTarget");

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
            .with(defaultRenderTarget, GL_COLOR_ATTACHMENT0)
            .with(pickingRenderTarget, GL_COLOR_ATTACHMENT1)
            .with(depthRenderTarget, GL_DEPTH_ATTACHMENT)
            .build();

        auto controlFramebuffer = framebufferAllocator->newFramebuffer(controlFramebufferLayout, resolution);
        safeDelete(controlFramebufferLayout);

        auto pass = new renderPass(program, controlFramebuffer, resolution);
        pass->addVao(renderAdapter->getVao());

        pass->setOnInitialize([=]
        {
            updateGlassyUniformBlock(glassyUniformBlockBuffer, defaultRenderTarget, resolution);
        });

        pass->setOnBeginRender([=](phi::program* program, framebuffer* framebuffer, const phi::resolution& resolution)
        {
            controlFramebuffer->bindForDrawing();

            auto one = 1.0f;
            glClearBufferfv(GL_DEPTH, 0, &one);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBlendColor(1, 1, 1, 1);

            program->bind();

            if (texturesManager::getIsBindless())
                program->setUniform(0, texturesManager::handles);
            else
                program->setUniform(0, textureUnits::units);
        });

        pass->setOnRender([](const vector<vertexArrayObject*>& vaos)
        {
            for (auto vao : vaos)
                vao->render();
        });

        pass->setOnResize([=](const phi::resolution& resolution)
        {
            updateGlassyUniformBlock(glassyUniformBlockBuffer, defaultRenderTarget, resolution);
        });

        pass->setOnEndRender([controlFramebuffer, defaultRenderTarget](phi::program* program, framebuffer* framebuffer, const phi::resolution& resolution)
        {
            program->unbind();
            controlFramebuffer->blitToDefault(defaultRenderTarget, 0, 0, defaultRenderTarget->texture->w, defaultRenderTarget->texture->h);

            glBlendColor(0, 0, 0, 0);
            glDisable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
        });

        pass->setOnDelete([glassyUniformBlockBuffer]() mutable
        {
            safeDelete(glassyUniformBlockBuffer);
        });

        return pass;
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
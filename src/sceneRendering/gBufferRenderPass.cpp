#include <precompiled.h>

#include <rendering/framebufferLayoutBuilder.h>
#include <rendering/programBuilder.h>
#include <rendering/texturesManager.h>
#include <rendering/textureUnits.h>

#include "gBufferRenderPass.h"

namespace phi
{
    gBufferRenderPass::gBufferRenderPass(
        const resolution& resolution,
        meshRenderAdapter* meshAdapter,
        const string& shadersPath,
        framebufferAllocator* framebufferAllocator) :
        renderPass(resolution),
        _shadersPath(shadersPath),
        _meshAdapter(meshAdapter),
        _framebufferAllocator(framebufferAllocator)
    {
        auto pickingRenderTarget = _framebufferAllocator->getRenderTarget("pickingRenderTarget");

        auto depthRenderTarget = _framebufferAllocator->getRenderTarget("depthRenderTarget");

        auto gBufferFramebufferLayout = framebufferLayoutBuilder::newFramebufferLayout("gBufferFramebuffer")
            .with("gBuffer_rt0", GL_COLOR_ATTACHMENT0, GL_RGBA16F, GL_RGBA)
            .with("gBuffer_rt1", GL_COLOR_ATTACHMENT1, GL_RGBA16F, GL_RGBA)
            .with("gBuffer_rt2", GL_COLOR_ATTACHMENT2, GL_RGBA16F, GL_RGBA)
            .with(pickingRenderTarget, GL_COLOR_ATTACHMENT3)
            .with(depthRenderTarget, GL_DEPTH_ATTACHMENT)
            .build();

        _framebuffer = _framebufferAllocator->newFramebuffer(gBufferFramebufferLayout, _resolution);
        safeDelete(gBufferFramebufferLayout);

        _program = programBuilder::buildProgram(_shadersPath, "gBuffer", "gBuffer");
        _program->addBuffer(_meshAdapter->getMaterialRenderDataBuffer());

        auto batchAddedToken = _meshAdapter->onBatchAdded->assign([=](batch* batch)
        {
            addVao(batch->getVao());
        });

        _meshAdapter->onDelete([&]()
        {
            _meshAdapter->onBatchAdded->unassign(batchAddedToken);
        });
    }

    gBufferRenderPass::~gBufferRenderPass()
    {
    }

    void gBufferRenderPass::onInitialize()
    {
    }

    void gBufferRenderPass::onBeginRender() 
    {
        const auto selectionRenderTargetNumber = 3;
        auto selectionClearColor = 0.0f;

        _framebuffer->bindForDrawing();

        glClearColor(1.0f, 0.7f, 0.0f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearBufferfv(GL_COLOR, selectionRenderTargetNumber, &selectionClearColor);

        _program->bind();

        if (texturesManager::getIsBindless())
            _program->setUniform(0, texturesManager::textureArraysHandles);
        else
            _program->setUniform(0, texturesManager::textureArraysUnits);
    }

    void gBufferRenderPass::onEndRender()
    {
        _unused(_framebuffer);

        _program->unbind();

        auto w = static_cast<GLint>(_resolution.width);
        auto h = static_cast<GLint>(_resolution.height);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    }
}
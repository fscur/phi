#include <precompiled.h>
#include "gBufferRenderPass.h"

#include <rendering\textureUnits.h>
#include <rendering\texturesManager.h>
#include <rendering\programBuilder.h>
#include <rendering\framebufferLayoutBuilder.h>

namespace phi
{
    renderPass* gBufferRenderPass::configure(
        meshRenderAdapter* meshAdapter,
        const resolution& resolution,
        const string& shadersPath,
        framebufferAllocator* framebufferAllocator)
    {
        auto pickingRenderTarget = framebufferAllocator->getRenderTarget("pickingRenderTarget");

        auto depthRenderTarget = framebufferAllocator->getRenderTarget("depthRenderTarget");

        auto gBufferFramebufferLayout = framebufferLayoutBuilder::newFramebufferLayout("gBufferFramebuffer")
            .with("gBuffer_rt0", GL_COLOR_ATTACHMENT0, GL_RGBA16F, GL_RGBA)
            .with("gBuffer_rt1", GL_COLOR_ATTACHMENT1, GL_RGBA16F, GL_RGBA)
            .with("gBuffer_rt2", GL_COLOR_ATTACHMENT2, GL_RGBA16F, GL_RGBA)
            .with(pickingRenderTarget, GL_COLOR_ATTACHMENT3)
            .with(depthRenderTarget, GL_DEPTH_ATTACHMENT)
            .build();

        auto gBufferFramebuffer = framebufferAllocator->newFramebuffer(gBufferFramebufferLayout, resolution);
        safeDelete(gBufferFramebufferLayout);

        auto program = programBuilder::buildProgram(shadersPath, "gBuffer", "gBuffer");
        program->addBuffer(meshAdapter->getMaterialRenderDataBuffer());

        auto pass = new renderPass(program, gBufferFramebuffer, resolution);

        auto batchAddedToken = meshAdapter->onBatchAdded->assign([=](batch* batch)
        {
            pass->addVao(batch->getVao());
        });

        meshAdapter->onDelete([meshAdapter, batchAddedToken]()
        {
            meshAdapter->onBatchAdded->unassign(batchAddedToken);
        });

        pass->setOnBeginRender([=](phi::program* program, framebuffer* framebuffer, const phi::resolution& resolution)
        {
            const auto selectionRenderTargetNumber = 3;
            auto selectionClearColor = 0.0f;

            framebuffer->bindForDrawing();

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glClearBufferfv(GL_COLOR, selectionRenderTargetNumber, &selectionClearColor);

            program->bind();

            if (texturesManager::getIsBindless())
                program->setUniform(0, texturesManager::handles);
            else
                program->setUniform(0, textureUnits::units);
        });

        pass->setOnRender([=](const vector<vertexArrayObject*>& vaos)
        {
            for (auto vao : vaos)
                vao->render();
        });

        pass->setOnEndRender([=](phi::program* program, framebuffer* framebuffer, const phi::resolution& resolution)
        {
            program->unbind();

            auto w = static_cast<GLint>(resolution.width);
            auto h = static_cast<GLint>(resolution.height);

            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        });

        return pass;
    }
}
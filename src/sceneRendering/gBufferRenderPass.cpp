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
        auto gBufferFramebufferLayout = framebufferLayoutBuilder::newFramebufferLayout("gBufferFramebuffer")
            .with("rt0", GL_COLOR_ATTACHMENT0, GL_RGBA16F, GL_RGBA)
            .with("rt1", GL_COLOR_ATTACHMENT1, GL_RGBA16F, GL_RGBA)
            .with("rt2", GL_COLOR_ATTACHMENT2, GL_RGBA16F, GL_RGBA)
            .with(pickingRenderTarget, GL_COLOR_ATTACHMENT3)
            .with("depth", GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT)
            .build();

        auto gBufferFramebuffer = framebufferAllocator->newFramebuffer(gBufferFramebufferLayout, resolution);

        auto program = programBuilder::buildProgram(shadersPath, "gBuffer", "gBuffer");
        program->addBuffer(meshAdapter->getMaterialRenderDataBuffer());

        auto pass = new renderPass(program, gBufferFramebuffer, resolution);
        auto renderTargets = gBufferFramebuffer->getRenderTargets();

        for (auto renderTarget : renderTargets)
            pass->addOut(renderTarget);

        meshAdapter->onBatchAdded->assign([=](batch* batch)
        {
            pass->addVao(batch->getVao());
        });

        pass->setOnBeginRender([=] (phi::program* program, framebuffer* framebuffer, const phi::resolution& resolution)
        {
            framebuffer->bindForDrawing();

            const auto selectionRenderTargetNumber = 3;
            auto selectionClearColor = 0.0f;

            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDepthMask(GL_TRUE);
            glEnable(GL_DEPTH_TEST);
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

            framebuffer->unbind(GL_FRAMEBUFFER);
            framebuffer->bindForReading();

            auto w = static_cast<GLint>(resolution.width);
            auto h = static_cast<GLint>(resolution.height);

            glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        });

        return pass;
    }
}
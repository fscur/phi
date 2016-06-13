#include <precompiled.h>
#include "gBufferRenderPass.h"

#include <rendering\texturesManager.h>
#include <rendering\programBuilder.h>
#include <rendering\framebufferBuilder.h>

namespace phi
{
    renderPass* gBufferRenderPass::configure(
        meshRendererDescriptor* rendererDescriptor,
        const resolution& resolution,
        const string& shadersPath)
    {
        auto gBufferFrameBuffer = framebufferBuilder::newFramebuffer(resolution)
            .with("rt0", GL_COLOR_ATTACHMENT0, GL_RGBA16F, GL_RGBA)
            .with("rt1", GL_COLOR_ATTACHMENT1, GL_RGBA16F, GL_RGBA)
            .with("rt2", GL_COLOR_ATTACHMENT2, GL_RGBA16F, GL_RGBA)
            .with("depth", GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT)
            .build();

        auto pickingFramebuffer = framebuffer::getPickingFramebuffer();
        auto pickingRenderTarget = pickingFramebuffer->getRenderTarget("pickingRenderTarget");
        gBufferFrameBuffer->add(GL_COLOR_ATTACHMENT3, pickingRenderTarget);

        auto program = programBuilder::buildProgram(shadersPath, "gBuffer", "gBuffer");
        program->addBuffer(rendererDescriptor->_materialRenderDataBuffer);

        auto pass = new renderPass(program, gBufferFrameBuffer);
        auto renderTargets = gBufferFrameBuffer->getRenderTargets();

        for (auto renderTarget : renderTargets)
            pass->addOut(renderTarget);

        rendererDescriptor->onBatchAdded->assign([=](batch* batch)
        {
            pass->addVao(batch->getVao());
        });

        pass->setOnBeginRender([=] (phi::program* program, framebuffer* framebuffer)
        {
            framebuffer->bindForDrawing();

            const auto selectionRenderTargetNumber = 3;
            auto selectionClearColor = 0.0f;

            glDepthMask(GL_TRUE);
            glEnable(GL_DEPTH_TEST);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glClearBufferfv(GL_COLOR, selectionRenderTargetNumber, &selectionClearColor);

            program->bind();

            if (texturesManager::getIsBindless())
                program->setUniform(0, texturesManager::handles);
            else
                program->setUniform(0, texturesManager::units);
        });

        pass->setOnRender([=](const vector<vertexArrayObject*>& vaos)
        {
            for (auto vao : vaos)
                vao->render();
        });

        pass->setOnEndRender([=](phi::program* program, framebuffer* framebuffer)
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
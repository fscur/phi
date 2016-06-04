#include <precompiled.h>
#include "gBufferRenderPassConfigurator.h"

#include <core\notImplementedException.h>

#include <rendering\programBuilder.h>
#include <rendering\framebufferBuilder.h>

namespace phi
{
    gBufferRenderPassConfigurator::gBufferRenderPassConfigurator()
    {
        throw new notImplementedException();
    }

    renderPass* gBufferRenderPassConfigurator::configureNewGBuffer(meshRendererDescriptor* rendererDescriptor, gl* gl, float width, float height, string shadersPath)
    {
        auto gBufferFrameBuffer = framebufferBuilder::newFramebuffer(gl, width, height)
            .with(GL_COLOR_ATTACHMENT0, GL_RGBA16F, GL_RGBA)
            .with(GL_COLOR_ATTACHMENT1, GL_RGBA16F, GL_RGBA)
            .with(GL_COLOR_ATTACHMENT2, GL_RGBA16F, GL_RGBA)
            .with(GL_COLOR_ATTACHMENT3, GL_RGBA8, GL_RGBA)
            .with(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT)
            .build();

        auto gBufferProgram = programBuilder::buildProgram(shadersPath, "gBuffer", "gBuffer");
        auto pass = new renderPass(gBufferProgram, gBufferFrameBuffer);
        
        for (auto renderTarget : gBufferFrameBuffer->getRenderTargets())
            pass->addOut(new renderPassOut(renderTarget));

        pass->addBuffer(rendererDescriptor->_materialRenderDataBuffer);

        rendererDescriptor->onBatchAdded->assign([=](batch* batch)
        {
            pass->addVao(batch);
        });

        pass->setOnBeginRender([=] (program* program, framebuffer* framebuffer)
        {
            framebuffer->bindForDrawing();

            const auto selectionRenderTargetNumber = 3;
            auto selectionClearColor = 0.0f;

            glDepthMask(GL_TRUE);
            glError::check();
            glEnable(GL_DEPTH_TEST);
            glError::check();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glError::check();
            glClearBufferfv(GL_COLOR, selectionRenderTargetNumber, &selectionClearColor);
            glError::check();

            program->bind();

            if (gl->currentState.useBindlessTextures)
                program->setUniform(0, gl->texturesManager->handles);
            else
                program->setUniform(0, gl->texturesManager->units);
        });

        pass->setOnRender([=](const vector<vao*>& vaos)
        {
            for (auto vao : vaos)
            {
                vao->bind();
                vao->render();
                vao->unbind();
            }
        });

        pass->setOnEndRender([=](program* program, framebuffer* framebuffer)
        {
            program->unbind();

            framebuffer->unbind(GL_FRAMEBUFFER);
            framebuffer->bindForReading();
            auto w = static_cast<GLint>(width);
            auto h = static_cast<GLint>(height);

            glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
            glError::check();
        });

        return pass;
    }
}
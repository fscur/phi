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
        auto program = programBuilder::buildProgram(shadersPath, "control", "control");
        program->addBuffer(renderAdapter->getControlRenderDataBuffer());
        
        auto pickingRenderTarget = framebufferAllocator->getRenderTarget("pickingRenderTarget");

        auto defaultFramebuffer = framebufferAllocator->getFramebuffer("defaultFramebuffer");
        auto defaultRenderTarget = defaultFramebuffer->getRenderTarget("defaultRenderTarget");

        auto controlFramebufferLayout = framebufferLayoutBuilder::newFramebufferLayout("controlFrameBuffer")
            .with(defaultRenderTarget, GL_COLOR_ATTACHMENT0)
            .with(pickingRenderTarget, GL_COLOR_ATTACHMENT1)
            .build();

        auto controlFramebuffer = framebufferAllocator->newFramebuffer(controlFramebufferLayout, resolution);
        safeDelete(controlFramebufferLayout);

        auto pass = new renderPass(program, controlFramebuffer, resolution);
        pass->addVao(renderAdapter->getVao());

        pass->setOnBeginRender([=](phi::program* program, framebuffer* framebuffer, const phi::resolution& resolution)
        {
            framebuffer->bindForDrawing();

            glEnable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBlendColor(1, 1, 1, 1);

            program->bind();

            if(texturesManager::getIsBindless())
                program->setUniform(0, texturesManager::handles);
            else
                program->setUniform(0, textureUnits::units);
        });

        pass->setOnRender([](const vector<vertexArrayObject*>& vaos)
        {
            for (auto vao : vaos)
                vao->render();
        });

        pass->setOnEndRender([controlFramebuffer, defaultRenderTarget](phi::program* program, framebuffer* framebuffer, const phi::resolution& resolution)
        {
            program->unbind();
            controlFramebuffer->blitToDefault(defaultRenderTarget, 0, 0, defaultRenderTarget->texture->w, defaultRenderTarget->texture->h);

            glBlendColor(0, 0, 0, 0);
            glDisable(GL_BLEND);
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
        });

        return pass;
    }
}
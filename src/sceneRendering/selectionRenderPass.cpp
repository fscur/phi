#include <precompiled.h>
#include "selectionRenderPass.h"

#include <core\time.h>
#include <core\geometry.h>

#include <rendering\textureUnits.h>
#include <rendering\texturesManager.h>
#include <rendering\gBufferRTAddresses.h>
#include <rendering\textureAddress.h>
#include <rendering\vertexArrayObject.h>

#include <rendering\programBuilder.h>
#include <rendering\framebufferLayoutBuilder.h>

namespace phi
{
    renderPass* selectionRenderPass::configure(
        const renderPass* lightingRenderPass, 
        const resolution& resolution, 
        const string& shadersPath,
        framebufferAllocator* framebufferAllocator)
    {
        auto pickingRenderTarget = framebufferAllocator->getRenderTarget("pickingRenderTarget");
        auto defaultFramebuffer = framebufferAllocator->getFramebuffer("defaultFramebuffer");
        auto defaultRenderTarget = defaultFramebuffer->getRenderTarget("defaultRenderTarget");

        auto quadVao = vertexArrayObject::createPostProcessVao();
        auto selectionProgram = programBuilder::buildProgram(shadersPath, "selection", "selection");
        
        auto pass = new renderPass(selectionProgram, defaultFramebuffer, resolution);
        pass->addVao(quadVao);

        pass->setOnBeginRender([=](program* program, framebuffer* framebuffer, const phi::resolution& resolution)
        {
            framebuffer->bindForDrawing();

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            program->bind();

            auto pickingRenderTargetAddress = texturesManager::getTextureAddress(pickingRenderTarget->texture);

            program->setUniform(0, resolution.toVec2());
            program->setUniform(1, 1.0f);
            program->setUniform(2, static_cast<float>(time::totalSeconds));
            program->setUniform(3, pickingRenderTargetAddress.unit);
            program->setUniform(4, pickingRenderTargetAddress.page);

            if (texturesManager::getIsBindless())
                program->setUniform(5, texturesManager::handles);
            else
                program->setUniform(5, textureUnits::units);
        });

        pass->setOnRender([=](const vector<vertexArrayObject*>& vaos)
        {
            for (auto vao : vaos)
                vao->render();
        });

        pass->setOnEndRender([=](phi::program* program, framebuffer* framebuffer, const phi::resolution& resolution)
        {
            program->unbind();
            glDisable(GL_BLEND);
        });

        pass->setOnDelete([quadVao] () mutable
        {
            safeDelete(quadVao);
        });

        return pass;
    }
}
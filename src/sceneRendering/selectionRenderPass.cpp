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
        const resolution& resolution, 
        const string& shadersPath,
        framebufferAllocator* framebufferAllocator)
    {
        auto pickingRenderTarget = framebufferAllocator->getRenderTarget("pickingRenderTarget");
        auto defaultFramebuffer = framebufferAllocator->getFramebuffer("defaultFramebuffer");

        auto quadVao = vertexArrayObject::createPostProcessVao();
        auto selectionProgram = programBuilder::buildProgram(shadersPath, "selection", "selection");
        
        auto pass = new renderPass(selectionProgram, defaultFramebuffer, resolution);
        pass->addVao(quadVao);

        pass->setOnCanRender([=]()
        {
            return true;
        });

        pass->setOnBeginRender([=](program* program, framebuffer* framebuffer, const phi::resolution& resolution)
        {
            framebuffer->bindForDrawing();

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            program->bind();

            auto pickingRenderTargetAddress = texturesManager::getTextureAddress(pickingRenderTarget->texture);

            program->setUniform(0, resolution.toVec2());
            program->setUniform(1, pickingRenderTargetAddress.unit);
            program->setUniform(2, pickingRenderTargetAddress.page);

            if (texturesManager::getIsBindless())
                program->setUniform(3, texturesManager::textureArraysHandles);
            else
                program->setUniform(3, texturesManager::textureArraysUnits);
        });

        pass->setOnRender([=](const vector<vertexArrayObject*>& vaos)
        {
            for (auto vao : vaos)
                vao->render();
        });

        pass->setOnEndRender([=](phi::program* program, framebuffer* framebuffer, const phi::resolution& resolution)
        {
            _unused(resolution);
            _unused(framebuffer);

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
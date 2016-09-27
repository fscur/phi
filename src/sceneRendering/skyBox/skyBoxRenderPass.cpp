#include <precompiled.h>
#include "skyBoxRenderPass.h"

#include <core\time.h>
#include <core\geometry.h>

#include <rendering\textureUnits.h>
#include <rendering\texturesManager.h>
#include <rendering\gBufferRTAddresses.h>
#include <rendering\textureAddress.h>
#include <rendering\vertexArrayObject.h>

#include <rendering\programBuilder.h>
#include <rendering\framebufferLayoutBuilder.h>
#include <rendering\vertexBuffer.h>
#include <rendering\indexBuffer.h>

namespace phi
{
    renderPass* skyBoxRenderPass::configure(
        const skyBoxRenderAdapter* renderAdapter,
        const resolution& resolution,
        const string& shadersPath,
        framebufferAllocator* framebufferAllocator)
    {
        auto defaultFramebuffer = framebufferAllocator->getFramebuffer("defaultFramebuffer");
        auto defaultRenderTarget = defaultFramebuffer->getRenderTarget("defaultRenderTarget");

        auto planeGridProgram = programBuilder::buildProgram(shadersPath, "skyBox", "skyBox");
        planeGridProgram->addBuffer(renderAdapter->getSkyBoxDataBuffer());

        auto pass = new renderPass(planeGridProgram, defaultFramebuffer, resolution);
        pass->addVao(renderAdapter->getVao());

        pass->setOnBeginRender([=](program* program, framebuffer* framebuffer, const phi::resolution& resolution)
        {
            framebuffer->bindForDrawing();

            program->bind();

            if (texturesManager::getIsBindless())
                program->setUniform(0, texturesManager::handles);
            else
                program->setUniform(0, textureUnits::units);

            glEnable(GL_DEPTH_TEST);

        });

        pass->setOnRender([=](const vector<vertexArrayObject*>& vaos)
        {
            for (auto vao : vaos)
                vao->render();
        });

        pass->setOnEndRender([=](phi::program* program, framebuffer* framebuffer, const phi::resolution& resolution)
        {
            program->unbind();

            glDisable(GL_DEPTH_TEST);
        });

        return pass;
    }
}
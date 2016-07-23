#include <precompiled.h>
#include "planeGridRenderPass.h"

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
    renderPass* planeGridRenderPass::configure(
        const planeGridRenderAdapter* renderAdapter,
        const resolution& resolution,
        const string& shadersPath,
        framebufferAllocator* framebufferAllocator)
    {
        //auto finalImageFramebuffer = framebufferAllocator->getFramebuffer("finalImageFramebuffer");
        //auto finalImageRenderTarget = finalImageFramebuffer->getRenderTarget("finalImageRenderTarget");

        auto planeGridProgram = programBuilder::buildProgram(shadersPath, "planeGrid", "planeGrid");
        planeGridProgram->addBuffer(renderAdapter->getPlaneGridRenderDataBuffer());


        auto pass = new renderPass(planeGridProgram, framebuffer::defaultFramebuffer, resolution);
        pass->addVao(renderAdapter->getVao());
        


        pass->setOnBeginRender([=](program* program, framebuffer* framebuffer, const phi::resolution& resolution)
        {
            framebuffer->bindForDrawing();
            //glClearColor(0.0, 0.0, 0.0, 1.0);
            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDisable(GL_CULL_FACE);
            glDepthMask(GL_FALSE);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            
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

            glDisable(GL_BLEND);
            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
            glEnable(GL_CULL_FACE);
        });

        return pass;
    }
}
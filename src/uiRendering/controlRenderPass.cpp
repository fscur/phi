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
        _unused(framebufferAllocator);

        auto program = programBuilder::buildProgram(shadersPath, "control", "control");
        program->addBuffer(renderAdapter->getControlRenderDataBuffer());

        auto pass = new renderPass(program, framebuffer::defaultFramebuffer, resolution);
        pass->addVao(renderAdapter->getVao());

        pass->setOnBeginRender([=](phi::program* program, framebuffer* framebuffer, const phi::resolution& resolution)
        {
            _unused(resolution);

            framebuffer->bindForDrawing();

            glDisable(GL_DEPTH_TEST);
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

        pass->setOnEndRender([](phi::program* program, framebuffer* framebuffer, const phi::resolution& resolution)
        {
            _unused(framebuffer);
            _unused(resolution);

            program->unbind();

            glBlendColor(0, 0, 0, 0);
            glDisable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
        });

        return pass;
    }
}
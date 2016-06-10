#include <precompiled.h>
#include "textRenderPass.h"

#include <rendering\texturesManager.h>
#include <rendering\programBuilder.h>

namespace phi
{
    renderPass * textRenderPass::configure(textRendererDescriptor* rendererDescriptor, const string& shadersPath)
    {
        auto program = programBuilder::buildProgram(shadersPath, "text", "text");
        program->addBuffer(rendererDescriptor->_glyphRenderDataBuffer);
        
        auto pass = new renderPass(program, framebuffer::defaultFramebuffer);
        pass->addVao(rendererDescriptor->_vao);

        pass->setOnBeginRender([=](phi::program* program, framebuffer* framebuffer)
        {
            framebuffer->bindForDrawing();

            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBlendColor(1, 1, 1, 1);

            program->bind();

            if (texturesManager::getIsBindless())//TODO: AAAAAAAAAAAAA ARRUMA LAMBDAS SOMETHING
                program->setUniform(0, texturesManager::handles);
            else
                program->setUniform(0, texturesManager::units);
        });

        pass->setOnRender([](const vector<vertexArrayObject*>& vaos)
        {
            for (auto vao : vaos)
                vao->render();
        });

        pass->setOnEndRender([](phi::program* program, framebuffer* framebuffer)
        {
            program->unbind();

            glBlendColor(0, 0, 0, 0);
            glDisable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
        });

        return pass;
    }
}
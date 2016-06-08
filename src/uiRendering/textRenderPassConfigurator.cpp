#include <precompiled.h>
#include "textRenderPassConfigurator.h"

#include <rendering\programBuilder.h>

namespace phi
{
    textRenderPassConfigurator::textRenderPassConfigurator()
    {
    }

    renderPass * textRenderPassConfigurator::configureNewTextRenderPass(textRendererDescriptor* rendererDescriptor, gl* gl, string shadersPath)
    {
        auto controlProgram = programBuilder::buildProgram(shadersPath, "text", "text");

        auto defaultFrameBuffer = new framebuffer(true);

        auto pass = new renderPass(controlProgram, defaultFrameBuffer);

        pass->addBuffer(rendererDescriptor->_glyphRenderDataBuffer);
        pass->addVao(rendererDescriptor->_vao);

        pass->setOnBeginRender([=](program* program, framebuffer* framebuffer)
        {
            framebuffer->bindForDrawing();

            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBlendColor(1, 1, 1, 1);

            program->bind();
            program->setUniform(0, gl->texturesManager->units);
        });

        pass->setOnRender([](const vector<vertexArrayObject*>& vaos)
        {
            for (auto vao : vaos)
                vao->render();
        });

        pass->setOnEndRender([](program* program, framebuffer* framebuffer)
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
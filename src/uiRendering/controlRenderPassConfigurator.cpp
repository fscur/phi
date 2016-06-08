#include <precompiled.h>
#include "controlRenderPassConfigurator.h"

#include <rendering\vertexArrayObject.h>
#include <rendering\programBuilder.h>
#include <rendering\framebufferBuilder.h>

namespace phi
{
    renderPass * controlRenderPassConfigurator::configureNewControlRenderPass(controlRendererDescriptor* rendererDescriptor, gl* gl, string shadersPath)
    {
        auto program = programBuilder::buildProgram(shadersPath, "control", "control");
        program->addBuffer(rendererDescriptor->_controlsRenderDataBuffer);

        auto defaultFrameBuffer = new framebuffer(true);

        auto pass = new renderPass(program, defaultFrameBuffer);
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
            program->setUniform(0, gl->texturesManager->units);
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
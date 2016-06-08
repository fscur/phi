#include <precompiled.h>
#include "glassyControlRenderPassConfigurator.h"

#include <rendering\vertexArrayObject.h>
#include <rendering\programBuilder.h>
#include <rendering\framebufferBuilder.h>

namespace phi
{
    renderPass* glassyControlRenderPassConfigurator::configureNewGlassyControlRenderPass(
        glassyControlRendererDescriptor* rendererDescriptor,
        gl* gl,
        float width,
        float height,
        string shadersPath)
    {
        auto defaultFrameBuffer = new framebuffer(true);
        
        auto program = programBuilder::buildProgram(shadersPath, "control", "glassy");
        program->addBuffer(rendererDescriptor->_controlsRenderDataBuffer);
        program->addBuffer(rendererDescriptor->_uniformBlockBuffer);
        
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
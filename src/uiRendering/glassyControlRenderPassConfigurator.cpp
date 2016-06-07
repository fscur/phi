#include <precompiled.h>
#include "glassyControlRenderPassConfigurator.h"

#include <rendering\vertexArrayObject.h>
#include <rendering\programBuilder.h>
#include <rendering\framebufferBuilder.h>

namespace phi
{
    renderPass* glassyControlRenderPassConfigurator::configureNewGlassyControlRenderPass(
        renderPass* lastRenderPass,
        glassyControlRendererDescriptor* rendererDescriptor, 
        gl* gl, 
        float width, 
        float height, 
        string shadersPath)
    {
        struct glassyControlUniformBlockData
        {
            vec2 resolution;
            float backgroundPage;
            float pad0;
            int backgroundUnit;
            int level;
            int pad1;
            int pad2;
        };

        auto uniformBlockBuffer = new buffer(bufferTarget::uniform);
        auto uniformBlockData = glassyControlUniformBlockData();
        auto rtAddress = lastRenderPass->getOuts().back()->renderTarget->textureAddress;
        uniformBlockData.backgroundPage = rtAddress.page;
        uniformBlockData.backgroundUnit = rtAddress.unit;
        uniformBlockData.level = 2;
        uniformBlockData.resolution = vec2(width, height);

        uniformBlockBuffer->storage(
            sizeof(glassyControlUniformBlockData),
            &uniformBlockData,
            bufferStorageUsage::write);
        uniformBlockBuffer->bindBufferBase(2);

        auto controlProgram = programBuilder::buildProgram(shadersPath, "control", "glassy");
        //auto controlFrameBuffer = framebufferBuilder::newFramebuffer(gl, width, height)
        //    .with(GL_COLOR_ATTACHMENT0, GL_RGBA8, GL_RGBA)
        //    .build();

        auto defaultFrameBuffer = new framebuffer(true);

        auto pass = new renderPass(controlProgram, defaultFrameBuffer);

        pass->addBuffer(rendererDescriptor->_controlsRenderDataBuffer);
        pass->addBuffer(uniformBlockBuffer);
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
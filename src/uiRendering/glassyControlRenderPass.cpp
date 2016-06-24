#include <precompiled.h>
#include "glassyControlRenderPass.h"

#include <rendering\texturesManager.h>
#include <rendering\textureUnits.h>
#include <rendering\vertexArrayObject.h>
#include <rendering\programBuilder.h>
#include <rendering\framebufferLayoutBuilder.h>

namespace phi
{
    void glassyControlRenderPass::updateUniformBlock(buffer* buffer, renderTarget* finalImageRenderTarget, const resolution& resolution)
    {
        auto uniformBlockData = glassyControlUniformBlockData();
        auto rtAddress = texturesManager::getTextureAddress(finalImageRenderTarget->texture);
        uniformBlockData.backgroundPage = rtAddress.page;
        uniformBlockData.backgroundUnit = rtAddress.unit;
        uniformBlockData.level = 2;
        uniformBlockData.resolution = resolution.toVec2();

        buffer->data(
            sizeof(glassyControlUniformBlockData),
            &uniformBlockData,
            bufferDataUsage::dynamicDraw);
    }

    renderPass* glassyControlRenderPass::configure(
        controlRenderAdapter* renderAdapter, 
        const resolution& resolution,
        const string& shadersPath,
        framebufferAllocator* framebufferAllocator)
    {
        auto uniformBlockBuffer = new buffer("GlassyControlUniformBlock", bufferTarget::uniform);
        uniformBlockBuffer->data(
            sizeof(glassyControlUniformBlockData),
            nullptr,
            bufferDataUsage::dynamicDraw);

        auto program = programBuilder::buildProgram(shadersPath, "control", "glassy");
        program->addBuffer(renderAdapter->getControlRenderDataBuffer());
        program->addBuffer(uniformBlockBuffer);
        
        auto finalImageFramebuffer = framebufferAllocator->getFramebuffer("finalImageFramebuffer");
        auto finalImageRenderTarget = finalImageFramebuffer->getRenderTarget("finalImageRenderTarget");

        auto pass = new renderPass(program, framebuffer::defaultFramebuffer, resolution);
        pass->addVao(renderAdapter->getVao());
        
        pass->setOnInitialize([=] 
        {
            updateUniformBlock(uniformBlockBuffer, finalImageRenderTarget, resolution);
        });

        pass->setOnBeginRender([=](phi::program* program, framebuffer* framebuffer, const phi::resolution& resolution)
        {
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
            program->unbind();

            glBlendColor(0, 0, 0, 0);
            glDisable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
        });

        pass->setOnResize([=](const phi::resolution& resolution)
        {
            updateUniformBlock(uniformBlockBuffer, finalImageRenderTarget, resolution);
        });

        return pass;
    }
}
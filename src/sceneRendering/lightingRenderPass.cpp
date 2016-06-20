#include <precompiled.h>
#include "lightingRenderPass.h"

#include <core\notImplementedException.h>
#include <core\geometry.h>

#include <rendering\texturesManager.h>
#include <rendering\gBufferRTAddresses.h>
#include <rendering\textureAddress.h>
#include <rendering\vertexArrayObject.h>

#include <rendering\programBuilder.h>
#include <rendering\framebufferLayoutBuilder.h>

namespace phi
{
    void lightingRenderPass::updateGBufferRTsBuffer(
        buffer* rtsBuffer,
        const texture* rt0Texture,
        const texture* rt1Texture,
        const texture* rt2Texture,
        const texture* rtDepthTexture)
    {
        auto rtAddresses = phi::gBufferRTAddresses();
        auto rt0Address = texturesManager::getTextureAddress(rt0Texture);
        auto rt1Address = texturesManager::getTextureAddress(rt1Texture);
        auto rt2Address = texturesManager::getTextureAddress(rt2Texture);
        auto rtDepthAddress = texturesManager::getTextureAddress(rtDepthTexture);

        rtAddresses.rt0Unit = rt0Address.unit;
        rtAddresses.rt0Page = rt0Address.page;
        rtAddresses.rt1Unit = rt1Address.unit;
        rtAddresses.rt1Page = rt1Address.page;
        rtAddresses.rt2Unit = rt2Address.unit;
        rtAddresses.rt2Page = rt2Address.page;
        rtAddresses.depthUnit = rtDepthAddress.unit;
        rtAddresses.depthPage = rtDepthAddress.page;

        rtsBuffer->data(
            sizeof(gBufferRTAddresses),
            &rtAddresses,
            bufferDataUsage::dynamicDraw);
    }

    renderPass* lightingRenderPass::configure(
        renderPass* gBufferRenderPass, 
        const resolution& resolution, 
        const string& shadersPath,
        framebufferAllocator* framebufferAllocator)
    {
        auto renderTargets = gBufferRenderPass->getOuts();

        auto rt0 = renderTargets["rt0"];
        auto rt1 = renderTargets["rt1"];
        auto rt2 = renderTargets["rt2"];
        auto rtDepth = renderTargets["depth"];

        auto finalImageLayout = textureLayout();
        finalImageLayout.dataFormat = GL_RGBA;
        finalImageLayout.dataType = GL_UNSIGNED_BYTE;
        finalImageLayout.internalFormat = GL_RGBA8;
        finalImageLayout.wrapMode = GL_CLAMP_TO_EDGE;
        finalImageLayout.minFilter = GL_LINEAR_MIPMAP_LINEAR;
        finalImageLayout.magFilter = GL_LINEAR;

        auto finalImageFramebufferLayout = framebufferLayoutBuilder::newFramebufferLayout("finalImageFramebuffer")
            .with("finalImageRenderTarget", GL_COLOR_ATTACHMENT0, finalImageLayout)
            .build();

        auto finalImageFramebuffer = framebufferAllocator->newFramebuffer(finalImageFramebufferLayout, resolution);
        auto finalImageRenderTarget = finalImageFramebuffer->getRenderTarget("finalImageRenderTarget");

        auto quadVao = vertexArrayObject::createPostProcessVao();

        auto rtsBuffer = new buffer("gBufferRTAddresses", bufferTarget::uniform);

        rtsBuffer->data(
            sizeof(gBufferRTAddresses),
            nullptr,
            bufferDataUsage::dynamicDraw);

        auto program = programBuilder::buildProgram(shadersPath, "lighting", "lighting");
        program->addBuffer(rtsBuffer);

        auto pass = new renderPass(program, finalImageFramebuffer, resolution);
        pass->addVao(quadVao);

        pass->addOut(rt0);
        pass->addOut(rt1);
        pass->addOut(rt2);
        pass->addOut(rtDepth);
        pass->addOut(finalImageRenderTarget);

        pass->setOnInitialize([=]()
        {
            updateGBufferRTsBuffer(
                rtsBuffer,
                rt0->texture,
                rt1->texture,
                rt2->texture,
                rtDepth->texture);
        });

        pass->setOnBeginRender([=](phi::program* program, framebuffer* framebuffer, const phi::resolution& resolution)
        {
            framebuffer->bindForDrawing();
            glDisable(GL_DEPTH_TEST);
            glClear(GL_COLOR_BUFFER_BIT);

            program->bind();

            if (texturesManager::getIsBindless())
                program->setUniform(0, texturesManager::handles);
            else
                program->setUniform(0, texturesManager::units);
        });

        pass->setOnRender([=](const vector<vertexArrayObject*>& vaos)
        {
            for (auto vao : vaos)
                vao->render();
        });

        pass->setOnEndRender([=](phi::program* program, framebuffer* framebuffer)
        {
            program->unbind();
            glEnable(GL_DEPTH_TEST);
        });

        pass->setOnResize([=](const phi::resolution& resolution)
        {
            updateGBufferRTsBuffer(
                rtsBuffer,
                rt0->texture,
                rt1->texture,
                rt2->texture,
                rtDepth->texture);
        });

        return pass;
    }
}
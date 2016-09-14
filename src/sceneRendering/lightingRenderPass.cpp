#include <precompiled.h>

#include <core/geometry.h>
#include <core/notImplementedException.h>

#include <rendering/framebufferLayoutBuilder.h>
#include <rendering/gBufferRTAddresses.h>
#include <rendering/programBuilder.h>
#include <rendering/textureAddress.h>
#include <rendering/texturesManager.h>
#include <rendering/textureUnits.h>
#include <rendering/vertexArrayObject.h>

#include "lightingRenderPass.h"

namespace phi
{
    renderPass* lightingRenderPass::configure(
        renderPass* gBufferRenderPass,
        const resolution& resolution,
        const string& shadersPath,
        framebufferAllocator* framebufferAllocator)
    {
        auto rt0 = framebufferAllocator->getRenderTarget("gBuffer_rt0");
        auto rt1 = framebufferAllocator->getRenderTarget("gBuffer_rt1");
        auto rt2 = framebufferAllocator->getRenderTarget("gBuffer_rt2");
        auto rtDepth = framebufferAllocator->getRenderTarget("depthRenderTarget");
        auto rtPicking = framebufferAllocator->getRenderTarget("pickingRenderTarget");

        auto quadVao = vertexArrayObject::createPostProcessVao();

        auto rtsBuffer = new buffer("gBufferRTAddresses", bufferTarget::uniform);

        /*rtsBuffer->data(
        sizeof(gBufferRTAddresses),
        nullptr,
        bufferDataUsage::dynamicDraw);*/

        auto program = programBuilder::buildProgram(shadersPath, "lighting", "lighting");
        program->addBuffer(rtsBuffer);

        auto defaultFramebuffer = framebufferAllocator->getFramebuffer("defaultFramebuffer");
        auto defaultRenderTarget = defaultFramebuffer->getRenderTarget("defaultRenderTarget");

        auto pass = new renderPass(program, defaultFramebuffer, resolution);
        pass->addVao(quadVao);

        pass->setOnInitialize([=]()
        {
            updateGBufferRTsBuffer(
                rtsBuffer,
                rt0->texture,
                rt1->texture,
                rt2->texture,
                rtDepth->texture,
                rtPicking->texture);
        });

        pass->setOnBeginRender([=](phi::program* program, framebuffer* framebuffer, const phi::resolution& resolution)
        {
            framebuffer->bindForDrawing();
            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);
            glClear(GL_COLOR_BUFFER_BIT);

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
        });

        pass->setOnResize([=](const phi::resolution& resolution)
        {
            updateGBufferRTsBuffer(
                rtsBuffer,
                rt0->texture,
                rt1->texture,
                rt2->texture,
                rtDepth->texture,
                rtPicking->texture);
        });

        pass->setOnDelete([rtsBuffer, quadVao]() mutable
        {
            safeDelete(rtsBuffer);
            safeDelete(quadVao);
        });

        return pass;
    }

    void lightingRenderPass::updateGBufferRTsBuffer(
        buffer* rtsBuffer,
        const texture* rt0Texture,
        const texture* rt1Texture,
        const texture* rt2Texture,
        const texture* rtDepthTexture,
        const texture* rtPickingTexture)
    {
        auto rtAddresses = phi::gBufferRTAddresses();
        auto rt0Address = texturesManager::getTextureAddress(rt0Texture);
        auto rt1Address = texturesManager::getTextureAddress(rt1Texture);
        auto rt2Address = texturesManager::getTextureAddress(rt2Texture);
        auto rtDepthAddress = texturesManager::getTextureAddress(rtDepthTexture);
        auto rtPickingAddress = texturesManager::getTextureAddress(rtPickingTexture);

        rtAddresses.rt0Unit = rt0Address.unit;
        rtAddresses.rt0Page = rt0Address.page;
        rtAddresses.rt1Unit = rt1Address.unit;
        rtAddresses.rt1Page = rt1Address.page;
        rtAddresses.rt2Unit = rt2Address.unit;
        rtAddresses.rt2Page = rt2Address.page;
        rtAddresses.depthUnit = rtDepthAddress.unit;
        rtAddresses.depthPage = rtDepthAddress.page;
        rtAddresses.pickingUnit = rtPickingAddress.unit;
        rtAddresses.pickingPage = rtPickingAddress.page;

        rtsBuffer->data(
            sizeof(gBufferRTAddresses),
            &rtAddresses,
            bufferDataUsage::dynamicDraw);
    }
}
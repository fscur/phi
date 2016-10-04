#include <precompiled.h>

#include <rendering/framebufferLayoutBuilder.h>
#include <rendering/gBufferRTAddresses.h>
#include <rendering/programBuilder.h>
#include <rendering/texturesManager.h>
#include <rendering/textureUnits.h>

#include "ghostMeshRenderPass.h"

namespace phi
{
    renderPass* ghostMeshRenderPass::configure(
        ghostMeshRenderAdapter* renderAdapter,
        const resolution& resolution,
        const string& shadersPath,
        framebufferAllocator* framebufferAllocator)
    {
        auto defaultFramebuffer = framebufferAllocator->getFramebuffer("defaultFramebuffer");
        auto defaultRenderTarget = defaultFramebuffer->getRenderTarget("defaultRenderTarget");

        auto rt0 = framebufferAllocator->getRenderTarget("gBuffer_rt0");
        auto rt1 = framebufferAllocator->getRenderTarget("gBuffer_rt1");
        auto rt2 = framebufferAllocator->getRenderTarget("gBuffer_rt2");
        auto pickingRenderTarget = framebufferAllocator->getRenderTarget("pickingRenderTarget");
        auto rtDepth = framebufferAllocator->getRenderTarget("depthRenderTarget");
        auto rtsBuffer = new buffer("gBufferRTAddresses", bufferTarget::uniform);

        /*rtsBuffer->data(
        sizeof(gBufferRTAddresses),
        nullptr,
        bufferDataUsage::dynamicDraw);*/
        auto ghostMeshProgram = programBuilder::buildProgram(shadersPath, "ghostMesh", "ghostMesh");
        ghostMeshProgram->addBuffer(rtsBuffer);

        auto pass = new renderPass(ghostMeshProgram, defaultFramebuffer, resolution);
        pass->addVao(renderAdapter->getVao());

        pass->setOnInitialize([=]()
        {
            createRTsBuffer(
                rtsBuffer,
                rt0->texture,
                rt1->texture,
                rt2->texture,
                rtDepth->texture,
                pickingRenderTarget->texture);
        });

        pass->setOnCanRender([=]()
        {
            return !renderAdapter->isEmpty();
        });

        pass->setOnBeginRender([=](program* program, framebuffer* framebuffer, const phi::resolution& resolution)
        {
            framebuffer->bindForDrawing();
            glDepthMask(GL_FALSE);
            glEnable(GL_BLEND);
            //glCullFace(GL_FRONT);
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(-1.0f, 0.0f);

            program->bind();

            if (texturesManager::getIsBindless())
                program->setUniform(0, texturesManager::textureArraysHandles);
            else
                program->setUniform(0, texturesManager::textureArraysUnits);
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
            //glCullFace(GL_BACK);
            glDepthMask(GL_TRUE);
            glDisable(GL_POLYGON_OFFSET_FILL);
            framebuffer->unbind(GL_FRAMEBUFFER);

            auto w = static_cast<GLint>(resolution.width);
            auto h = static_cast<GLint>(resolution.height);

            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

            framebuffer->bindForReading(defaultRenderTarget);

            glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_LINEAR);

            auto address = texturesManager::getTextureAddress(defaultRenderTarget->texture);
            glActiveTexture(GL_TEXTURE0 + address.unit);
            glBindTexture(GL_TEXTURE_2D_ARRAY, address.arrayId);
            glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
        });

        pass->setOnDelete([rtsBuffer]() mutable
        {
            safeDelete(rtsBuffer);
        });

        return pass;
    }

    void ghostMeshRenderPass::createRTsBuffer(
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
#include <precompiled.h>

#include <rendering/framebufferLayoutBuilder.h>
#include <rendering/gBufferRTAddresses.h>
#include <rendering/programBuilder.h>
#include <rendering/texturesManager.h>
#include <rendering/textureUnits.h>

#include "ghostMeshRenderPass.h"

namespace phi
{
    ghostMeshRenderPass::ghostMeshRenderPass(
        ghostMeshRenderAdapter* renderAdapter,
        const resolution& resolution, 
        const string& shadersPath, 
        framebufferAllocator* framebufferAllocator) :
        renderPass(resolution)
    {
        _framebuffer = framebufferAllocator->getFramebuffer("defaultFramebuffer");
        _defaultRenderTarget = _framebuffer->getRenderTarget("defaultRenderTarget");

        _rt0 = framebufferAllocator->getRenderTarget("gBuffer_rt0");
        _rt1 = framebufferAllocator->getRenderTarget("gBuffer_rt1");
        _rt2 = framebufferAllocator->getRenderTarget("gBuffer_rt2");
        _rtPicking = framebufferAllocator->getRenderTarget("pickingRenderTarget");
        _rtDepth = framebufferAllocator->getRenderTarget("depthRenderTarget");
        _rtsBuffer = new buffer("gBufferRTAddresses", bufferTarget::uniform);

        _program = programBuilder::buildProgram(shadersPath, "ghostMesh", "ghostMesh");
        _program->addBuffer(_rtsBuffer);

        addVao(renderAdapter->getVao());
    }

    ghostMeshRenderPass::~ghostMeshRenderPass()
    {
        safeDelete(_rtsBuffer);
    }

    void ghostMeshRenderPass::onInitialize()
    {
        createRTsBuffer(
            _rtsBuffer,
            _rt0->texture,
            _rt1->texture,
            _rt2->texture,
            _rtDepth->texture,
            _rtPicking->texture);
    }

    void ghostMeshRenderPass::onBeginRender()
    {
        _framebuffer->bindForDrawing();
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-1.0f, 0.0f);

        _program->bind();

        if (texturesManager::getIsBindless())
            _program->setUniform(0, texturesManager::textureArraysHandles);
        else
            _program->setUniform(0, texturesManager::textureArraysUnits);
    }

    void ghostMeshRenderPass::onEndRender()
    {
        _program->unbind();

        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
        glDisable(GL_POLYGON_OFFSET_FILL);
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
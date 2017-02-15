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
    lightingRenderPass::lightingRenderPass(
        const resolution& resolution,
        const string& shadersPath,
        framebufferAllocator* framebufferAllocator) :
        renderPass(resolution),
        _shadersPath(shadersPath),
        _framebufferAllocator(framebufferAllocator)
    {
        _rt0 = _framebufferAllocator->getRenderTarget("gBuffer_rt0");
        _rt1 = _framebufferAllocator->getRenderTarget("gBuffer_rt1");
        _rt2 = _framebufferAllocator->getRenderTarget("gBuffer_rt2");
        _rtDepth = _framebufferAllocator->getRenderTarget("depthRenderTarget");
        _rtPicking = _framebufferAllocator->getRenderTarget("pickingRenderTarget");
        

        _rtsBuffer = new buffer("gBufferRTAddresses", bufferTarget::uniform);

        _program = programBuilder::buildProgram(_shadersPath, "lighting", "lighting");
        _program->addBuffer(_rtsBuffer);

        _framebuffer = _framebufferAllocator->getFramebuffer("defaultFramebuffer");

        _quadVao = vertexArrayObject::createPostProcessVao();
        addVao(_quadVao);
    }

    lightingRenderPass::~lightingRenderPass()
    {
        safeDelete(_quadVao);
        safeDelete(_rtsBuffer);
    }

    void lightingRenderPass::onInitialize()
    {
        updateGBufferRTsBuffer(
            _rtsBuffer,
            _rt0->texture,
            _rt1->texture,
            _rt2->texture,
            _rtDepth->texture,
            _rtPicking->texture);
    }

    void lightingRenderPass::onBeginRender()
    {
        _framebuffer->bindForDrawing();

        //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        glClear(GL_COLOR_BUFFER_BIT);

        _program->bind();

        if (texturesManager::getIsBindless())
            _program->setUniform(0, texturesManager::textureArraysHandles);
        else
            _program->setUniform(0, texturesManager::textureArraysUnits);
    }

    void lightingRenderPass::onEndRender()
    {
        _program->unbind();
    }

    void lightingRenderPass::onResize(const resolution& resolution)
    {
        _unused(resolution);

        updateGBufferRTsBuffer(
            _rtsBuffer,
            _rt0->texture,
            _rt1->texture,
            _rt2->texture,
            _rtDepth->texture,
            _rtPicking->texture);
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
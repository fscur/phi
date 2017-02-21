#include <precompiled.h>
#include "skyBoxRenderPass.h"

#include <rendering\programBuilder.h>

namespace phi
{
    skyBoxRenderPass::skyBoxRenderPass(
        skyBoxRenderAdapter* renderAdapter,
        const resolution& resolution,
        const string& shadersPath,
        framebufferAllocator* framebufferAllocator) :
        renderPass(resolution)
    {
        _framebuffer = framebufferAllocator->getFramebuffer("defaultFramebuffer");
        _defaultRenderTarget = _framebuffer->getRenderTarget("defaultRenderTarget");

        _program = programBuilder::buildProgram(shadersPath, "skyBox", "skyBox");
        _program->addBuffer(renderAdapter->getSkyBoxDataBuffer());

        addVao(renderAdapter->getVao());
    }

    skyBoxRenderPass::~skyBoxRenderPass()
    {
    }

    void skyBoxRenderPass::onBeginRender()
    {
        _framebuffer->bindForDrawing();

        _program->bind();

        if (texturesManager::getIsBindless())
            _program->setUniform(0, texturesManager::cubeMapArraysHandles);
        else
            _program->setUniform(0, texturesManager::cubeMapArraysUnits);

        glEnable(GL_DEPTH_TEST);
    }

    void skyBoxRenderPass::onEndRender()
    {
        glDisable(GL_DEPTH_TEST);
        _program->unbind();
    }
}
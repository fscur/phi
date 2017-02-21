#include <precompiled.h>
#include "selectionRenderPass.h"

#include <core\time.h>
#include <core\geometry.h>

#include <rendering\textureUnits.h>
#include <rendering\texturesManager.h>
#include <rendering\gBufferRTAddresses.h>
#include <rendering\textureAddress.h>
#include <rendering\vertexArrayObject.h>

#include <rendering\programBuilder.h>
#include <rendering\framebufferLayoutBuilder.h>

namespace phi
{
    selectionRenderPass::selectionRenderPass(
        const resolution& resolution, 
        const string& shadersPath, 
        framebufferAllocator* framebufferAllocator) :
        renderPass(resolution)
    {
        _framebuffer = framebufferAllocator->getFramebuffer("defaultFramebuffer");
        _defaultRenderTarget = _framebuffer->getRenderTarget("defaultRenderTarget");

        _pickingRenderTarget = framebufferAllocator->getRenderTarget("pickingRenderTarget");
        _program = programBuilder::buildProgram(shadersPath, "selection", "selection");

        _quadVao = vertexArrayObject::createPostProcessVao();
        addVao(_quadVao);
    }

    selectionRenderPass::~selectionRenderPass()
    {
        safeDelete(_quadVao);
    }

    void selectionRenderPass::onBeginRender()
    {
        _framebuffer->bindForDrawing();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        _program->bind();

        auto pickingRenderTargetAddress = texturesManager::getTextureAddress(_pickingRenderTarget->texture);

        _program->setUniform(0, _resolution.toVec2());
        _program->setUniform(1, pickingRenderTargetAddress.unit);
        _program->setUniform(2, pickingRenderTargetAddress.page);

        if (texturesManager::getIsBindless())
            _program->setUniform(3, texturesManager::textureArraysHandles);
        else
            _program->setUniform(3, texturesManager::textureArraysUnits);
    }

    void selectionRenderPass::onEndRender()
    {
        _program->unbind();
        glDisable(GL_BLEND);

        /*auto w = static_cast<GLint>(_resolution.width);
        auto h = static_cast<GLint>(_resolution.height);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        _framebuffer->bindForReading(_defaultRenderTarget);

        glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_LINEAR);*/

        /*auto address = texturesManager::getTextureAddress(_defaultRenderTarget->texture);
        glActiveTexture(GL_TEXTURE0 + address.unit);
        glBindTexture(GL_TEXTURE_2D_ARRAY, address.arrayId);
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);*/
    }
}
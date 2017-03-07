#include <precompiled.h>
#include "rotationPlaneGridRenderPass.h"

#include <core\time.h>
#include <core\geometry.h>

#include <rendering\textureUnits.h>
#include <rendering\texturesManager.h>
#include <rendering\gBufferRTAddresses.h>
#include <rendering\textureAddress.h>
#include <rendering\vertexArrayObject.h>

#include <rendering\programBuilder.h>
#include <rendering\framebufferLayoutBuilder.h>
#include <rendering\vertexBuffer.h>
#include <rendering\indexBuffer.h>

namespace phi
{
    rotationPlaneGridRenderPass::rotationPlaneGridRenderPass(
        rotationPlaneGridRenderAdapter* renderAdapter, 
        const resolution& resolution, 
        const string& shadersPath, 
        framebufferAllocator* framebufferAllocator) :
        renderPass(resolution)
    {
        _framebuffer = framebufferAllocator->getFramebuffer("defaultFramebuffer");
        _defaultRenderTarget = _framebuffer->getRenderTarget("defaultRenderTarget");

        _program = programBuilder::buildProgram(shadersPath, "rotationPlaneGrid", "rotationPlaneGrid");
        _program->addBuffer(renderAdapter->getMousePlaneGridRenderDataBuffer());

        addVao(renderAdapter->getVao());
    }

    rotationPlaneGridRenderPass::~rotationPlaneGridRenderPass()
    {
    }

    void rotationPlaneGridRenderPass::onBeginRender()
    {
        _framebuffer->bindForDrawing();
        glDisable(GL_CULL_FACE);
        glDepthMask(GL_FALSE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-1.0f, 0.0f);
        glEnable(GL_CLIP_DISTANCE0);
        glEnable(GL_CLIP_DISTANCE1);
        glEnable(GL_CLIP_DISTANCE2);

        _program->bind();

        /*if (texturesManager::getIsBindless())
            _program->setUniform(0, texturesManager::textureArraysHandles);
        else
            _program->setUniform(0, texturesManager::textureArraysUnits);*/
    }

    void rotationPlaneGridRenderPass::onEndRender()
    {
        _program->unbind();

        glDisable(GL_CLIP_DISTANCE2);
        glDisable(GL_CLIP_DISTANCE1);
        glDisable(GL_CLIP_DISTANCE0);

        glPolygonOffset(0.0f, 0.0f);
        glDisable(GL_POLYGON_OFFSET_FILL);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glEnable(GL_CULL_FACE);

        auto w = static_cast<GLint>(_resolution.width);
        auto h = static_cast<GLint>(_resolution.height);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        _framebuffer->bindForReading(_defaultRenderTarget);

        glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_LINEAR);

        auto address = texturesManager::getTextureAddress(_defaultRenderTarget->texture);
        glActiveTexture(GL_TEXTURE0 + address.unit);
        glBindTexture(GL_TEXTURE_2D_ARRAY, address.arrayId);
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    }
}
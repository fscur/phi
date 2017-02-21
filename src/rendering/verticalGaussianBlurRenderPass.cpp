#include <precompiled.h>
#include "verticalGaussianBlurRenderPass.h"

#include "programBuilder.h"
#include "framebufferLayoutBuilder.h"
#include "textureUnits.h"
#include "texturesManager.h"

namespace phi
{
    verticalGaussianBlurRenderPass::verticalGaussianBlurRenderPass(
        const resolution& resolution, 
        const string& shadersPath, 
        framebufferAllocator * framebufferAllocator) :
        renderPass(resolution)
    {
        _blurHRenderTarget = framebufferAllocator->getRenderTarget("blurHRenderTarget");

        _program = programBuilder::buildProgram(shadersPath, "blurV", "blurV");

        auto imageLayout = textureLayout();
        imageLayout.dataFormat = GL_RGBA;
        imageLayout.dataType = GL_UNSIGNED_BYTE;
        imageLayout.internalFormat = GL_RGBA8;
        imageLayout.wrapMode = GL_CLAMP_TO_EDGE;
        imageLayout.minFilter = GL_LINEAR_MIPMAP_LINEAR;
        imageLayout.magFilter = GL_LINEAR;

        auto framebufferLayout = framebufferLayoutBuilder::newFramebufferLayout("blurV")
            .with("blurVRenderTarget", GL_COLOR_ATTACHMENT0, imageLayout)
            .build();

        _framebuffer = framebufferAllocator->newFramebuffer(framebufferLayout, resolution);
        safeDelete(framebufferLayout);

        _quadVao = vertexArrayObject::createPostProcessVao();
        addVao(_quadVao);
    }

    verticalGaussianBlurRenderPass::~verticalGaussianBlurRenderPass()
    {
        safeDelete(_quadVao);
    }

    void verticalGaussianBlurRenderPass::onBeginRender()
    {
        _framebuffer->bindForDrawing();

        _program->bind();

        auto address = texturesManager::getTextureAddress(_blurHRenderTarget->texture);
        _program->setUniform(0, address.unit);
        _program->setUniform(1, address.page);
        _program->setUniform(2, _resolution.toVec2());

        if (texturesManager::getIsBindless())
            _program->setUniform(3, texturesManager::textureArraysHandles);
        else
            _program->setUniform(3, texturesManager::textureArraysUnits);
    }
    void verticalGaussianBlurRenderPass::onEndRender()
    {
        _program->unbind();

        _framebuffer->unbind(GL_FRAMEBUFFER);
        auto renderTarget = _framebuffer->getRenderTarget("blurVRenderTarget");
        _framebuffer->blitToDefault(renderTarget);

        auto address = texturesManager::getTextureAddress(renderTarget->texture);
        glActiveTexture(GL_TEXTURE0 + address.unit);
        glBindTexture(GL_TEXTURE_2D_ARRAY, address.arrayId);
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    }
}
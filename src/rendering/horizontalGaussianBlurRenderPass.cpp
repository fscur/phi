#include <precompiled.h>
#include "horizontalGaussianBlurRenderPass.h"

#include "framebufferLayoutBuilder.h"
#include "programBuilder.h"
#include "textureUnits.h"
#include "texturesManager.h"

namespace phi
{
    horizontalGaussianBlurRenderPass::horizontalGaussianBlurRenderPass(
        const resolution& resolution, 
        const string& shadersPath, 
        framebufferAllocator* framebufferAllocator) :
        renderPass(resolution)
    {
        _finalImageRenderTarget = framebufferAllocator->getRenderTarget("finalImageRenderTarget");

        _program = programBuilder::buildProgram(shadersPath, "blurH", "blurH");

        auto imageLayout = textureLayout();
        imageLayout.dataFormat = GL_RGBA;
        imageLayout.dataType = GL_UNSIGNED_BYTE;
        imageLayout.internalFormat = GL_RGBA8;
        imageLayout.wrapMode = GL_CLAMP_TO_EDGE;
        imageLayout.minFilter = GL_LINEAR_MIPMAP_LINEAR;
        imageLayout.magFilter = GL_LINEAR;

        auto framebufferLayout = framebufferLayoutBuilder::newFramebufferLayout("blurH")
            .with("blurHRenderTarget", GL_COLOR_ATTACHMENT0, imageLayout)
            .build();

        _framebuffer = framebufferAllocator->newFramebuffer(framebufferLayout, resolution);
        safeDelete(framebufferLayout);

        _quadVao = vertexArrayObject::createPostProcessVao();
        addVao(_quadVao);
    }

    horizontalGaussianBlurRenderPass::~horizontalGaussianBlurRenderPass()
    {
        safeDelete(_quadVao);
    }

    void horizontalGaussianBlurRenderPass::onBeginRender()
    {
        _framebuffer->bindForDrawing();

        _program->bind();

        auto address = texturesManager::getTextureAddress(_finalImageRenderTarget->texture);
        _program->setUniform(0, address.unit);
        _program->setUniform(1, address.page);
        _program->setUniform(2, _resolution.toVec2());

        if (texturesManager::getIsBindless())
            _program->setUniform(3, texturesManager::textureArraysHandles);
        else
            _program->setUniform(3, texturesManager::textureArraysUnits);
    }

    void horizontalGaussianBlurRenderPass::onEndRender()
    {
        _program->unbind();
        _framebuffer->unbind(GL_FRAMEBUFFER);
    }
}
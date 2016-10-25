#include <precompiled.h>
#include "horizontalGaussianBlurRenderPass.h"

#include "framebufferLayoutBuilder.h"
#include "programBuilder.h"
#include "textureUnits.h"
#include "texturesManager.h"

namespace phi
{
    renderPass* horizontalGaussianBlurRenderPass::configure(
        const resolution& resolution,
        const string & shadersPath,
        framebufferAllocator * framebufferAllocator)
    {
        auto finalImageRenderTarget = framebufferAllocator->getRenderTarget("finalImageRenderTarget");

        auto blurHProgram = programBuilder::buildProgram(shadersPath, "blurH", "blurH");

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

        auto blurHFramebuffer = framebufferAllocator->newFramebuffer(framebufferLayout, resolution);
        safeDelete(framebufferLayout);

        auto pass = new renderPass(blurHProgram, blurHFramebuffer, resolution);

        auto quadVao = vertexArrayObject::createPostProcessVao();

        pass->addVao(quadVao);

        pass->setOnBeginRender([=](program* program, framebuffer* framebuffer, const phi::resolution& resolution)
        {
            framebuffer->bindForDrawing();

            program->bind();

            auto address = texturesManager::getTextureAddress(finalImageRenderTarget->texture);
            program->setUniform(0, address.unit);
            program->setUniform(1, address.page);
            program->setUniform(2, resolution.toVec2());

            if (texturesManager::getIsBindless())
                program->setUniform(3, texturesManager::handles);
            else
                program->setUniform(3, textureUnits::units);
        });

        pass->setOnRender([=](const vector<vertexArrayObject*>& vaos)
        {
            for (auto vao : vaos)
                vao->render();
        });

        pass->setOnEndRender([=](phi::program* program, framebuffer* framebuffer, const phi::resolution resolution)
        {
            _unused(resolution);

            program->unbind();
            framebuffer->unbind(GL_FRAMEBUFFER);
        });

        return pass;
    }
}
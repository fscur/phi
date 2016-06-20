#include <precompiled.h>
#include "verticalGaussianBlurRenderPass.h"

#include "programBuilder.h"
#include "framebufferLayoutBuilder.h"
#include "textureUnits.h"

namespace phi
{
    renderPass* verticalGaussianBlurRenderPass::configure(
        const resolution& resolution,
        const string & shadersPath,
        framebufferAllocator* framebufferAllocator)
    {
        auto blurHRenderTarget = framebufferAllocator->getRenderTarget("blurHRenderTarget");

        auto blurVProgram = programBuilder::buildProgram(shadersPath, "blurV", "blurV");

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

        auto blurVFramebuffer = framebufferAllocator->newFramebuffer(framebufferLayout, resolution);

        auto pass = new renderPass(blurVProgram, blurVFramebuffer, resolution);

        auto quadVao = vertexArrayObject::createPostProcessVao();

        pass->addVao(quadVao);

        pass->setOnBeginRender([=](program* program, framebuffer* framebuffer, const phi::resolution& resolution)
        {
            framebuffer->bindForDrawing();

            program->bind();

            auto address = texturesManager::getTextureAddress(blurHRenderTarget->texture);
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
            program->unbind();

            framebuffer->unbind(GL_FRAMEBUFFER);
            auto renderTarget = framebuffer->getRenderTarget("blurVRenderTarget");
            framebuffer->blitToDefault(renderTarget);

            auto address = texturesManager::getTextureAddress(renderTarget->texture);
            glActiveTexture(GL_TEXTURE0 + address.unit);
            glBindTexture(GL_TEXTURE_2D_ARRAY, address.containerId);
            glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
        });

        return pass;
    }
}
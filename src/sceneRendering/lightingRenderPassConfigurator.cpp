#include <precompiled.h>
#include "lightingRenderPassConfigurator.h"

#include <core\notImplementedException.h>

#include <rendering\renderTargetsAddresses.h>
#include <rendering\textureAddress.h>

#include <rendering\postProcessVao.h>
#include <rendering\programBuilder.h>

namespace phi
{
    lightingRenderPassConfigurator::lightingRenderPassConfigurator()
    {
        throw new notImplementedException();
    }

    renderPass* lightingRenderPassConfigurator::configureNewLighting(renderPass* gBufferRenderPass, gl* gl, float width, float height, string& shadersPath)
    {
        auto renderTargets = gBufferRenderPass->getOuts();

        auto rtAddresses = new phi::renderTargetsAddresses();
        rtAddresses->rt0Unit = renderTargets[0]->renderTarget->textureAddress.unit;
        rtAddresses->rt1Unit = renderTargets[1]->renderTarget->textureAddress.unit;
        rtAddresses->rt2Unit = renderTargets[2]->renderTarget->textureAddress.unit;
        rtAddresses->rt3Unit = renderTargets[3]->renderTarget->textureAddress.unit;
        rtAddresses->depthUnit = renderTargets[4]->renderTarget->textureAddress.unit;
        rtAddresses->rt0Page = renderTargets[0]->renderTarget->textureAddress.page;
        rtAddresses->rt1Page = renderTargets[1]->renderTarget->textureAddress.page;
        rtAddresses->rt2Page = renderTargets[2]->renderTarget->textureAddress.page;
        rtAddresses->rt3Page = renderTargets[3]->renderTarget->textureAddress.page;
        rtAddresses->depthPage = renderTargets[4]->renderTarget->textureAddress.page;

        auto layout = textureLayout();
        layout.dataFormat = GL_RGBA;
        layout.dataType = GL_UNSIGNED_BYTE;
        layout.internalFormat = GL_RGBA8;
        layout.wrapMode = GL_CLAMP_TO_EDGE;
        layout.minFilter = GL_LINEAR_MIPMAP_LINEAR;
        layout.magFilter = GL_LINEAR;

        auto finalImageTexture = new phi::texture(
            static_cast<uint>(width),
            static_cast<uint>(height),
            layout,
            nullptr,
            true,
            false);

        auto finalImageTexAddress = gl->texturesManager->get(finalImageTexture);

        auto finalImageRT = new renderTarget(
            GL_COLOR_ATTACHMENT0,
            static_cast<GLint>(width),
            static_cast<GLint>(height),
            finalImageTexAddress,
            finalImageTexture);

        auto finalImageFramebuffer = new framebuffer();
        finalImageFramebuffer->add(finalImageRT);

        auto quadVao = new postProcessVao();

        auto rtsBuffer = new buffer(bufferTarget::uniform);

        rtsBuffer->storage(
            sizeof(renderTargetsAddresses),
            &*rtAddresses,
            bufferStorageUsage::write);
        rtsBuffer->bindBufferBase(2);

        auto lightingProgram = programBuilder::buildProgram(shadersPath, "lighting", "lighting");
        auto pass = new renderPass(lightingProgram, finalImageFramebuffer);
        
        pass->addVao(quadVao);
        pass->addBuffer(rtsBuffer);

        pass->setOnBeginRender([=](program* program, framebuffer* framebuffer)
        {
            framebuffer->bindForDrawing();

            glDisable(GL_DEPTH_TEST);
            glClear(GL_COLOR_BUFFER_BIT);

            program->bind();

            if (gl->currentState.useBindlessTextures)//TODO: AAAAAAAAAAAAA ARRUMA LAMBDAS SOMETHING
                program->setUniform(0, gl->texturesManager->handles);
            else
                program->setUniform(0, gl->texturesManager->units);
        });

        pass->setOnRender([=](const vector<vao*>& vaos)
        {
            for (auto vao : vaos)
            {
                vao->bind();
                vao->render();
                vao->unbind();
            }
        });

        pass->setOnEndRender([=](program* program, framebuffer* framebuffer)
        {
            program->unbind();
            glEnable(GL_DEPTH_TEST);

            framebuffer->blitToDefault(finalImageRT);
        });

        //TODO: finalImageRT should be an OUT

        return pass;
    }
}
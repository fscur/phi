#include <precompiled.h>
#include "lightingRenderPass.h"

#include <core\notImplementedException.h>
#include <core\geometry.h>

#include <rendering\texturesManager.h>
#include <rendering\renderTargetsAddresses.h>
#include <rendering\textureAddress.h>
#include <rendering\vertexArrayObject.h>

#include <rendering\programBuilder.h>

namespace phi
{
    renderPass* lightingRenderPass::configure(
        renderPass* gBufferRenderPass, 
        const resolution& resolution, 
        const string& shadersPath,
        framebufferAllocator* framebufferAllocator)
    {
        auto renderTargets = gBufferRenderPass->getOuts();

        auto rt0 = renderTargets["rt0"];
        auto rt1 = renderTargets["rt1"];
        auto rt2 = renderTargets["rt2"];
        //auto rt3 = renderTargets["pickingRenderTarget"];
        auto rtDepth = renderTargets["depth"];

        /*auto layout = textureLayout();
        layout.dataFormat = GL_RGBA;
        layout.dataType = GL_UNSIGNED_BYTE;
        layout.internalFormat = GL_RGBA8;
        layout.wrapMode = GL_CLAMP_TO_EDGE;
        layout.minFilter = GL_LINEAR_MIPMAP_LINEAR;
        layout.magFilter = GL_LINEAR;

        auto finalImageTexture = new phi::texture(
            static_cast<uint>(resolution.width),
            static_cast<uint>(resolution.height),
            layout,
            nullptr,
            true,
            false);

        auto finalImageTexAddress = texturesManager::add(finalImageTexture);

        auto finalImageRT = new renderTarget(
            "finalImageRenderTarget",
            static_cast<GLint>(resolution.width),
            static_cast<GLint>(resolution.height),
            layout,
            finalImageTexAddress);

        auto finalImageFramebuffer = new framebuffer();
        finalImageFramebuffer->add(finalImageRT, GL_COLOR_ATTACHMENT0);*/

        auto quadVao = vertexArrayObject::createPostProcessVao();

        auto rtsBuffer = new buffer("RenderTargetAddresses", bufferTarget::uniform);

        rtsBuffer->data(
            sizeof(renderTargetsAddresses),
            nullptr,
            bufferDataUsage::dynamicDraw);

        auto program = programBuilder::buildProgram(shadersPath, "lighting", "lighting");
        program->addBuffer(rtsBuffer);

        auto pass = new renderPass(program, framebuffer::defaultFramebuffer);
        pass->addVao(quadVao);

        pass->addOut(rt0);
        pass->addOut(rt1);
        pass->addOut(rt2);
        //pass->addOut(rt3);
        pass->addOut(rtDepth);
        //pass->addOut(finalImageRT);

        pass->setOnInitialize([=]
        {
            auto rtAddresses = phi::renderTargetsAddresses();
            auto rt0Address = texturesManager::getTextureAddress(rt0->texture);
            auto rt1Address = texturesManager::getTextureAddress(rt1->texture);
            auto rt2Address = texturesManager::getTextureAddress(rt2->texture);
            //auto rt3Address = texturesManager::getTextureAddress(rt3->texture);
            auto rtDepthAddress = texturesManager::getTextureAddress(rtDepth->texture);

            rtAddresses.rt0Unit = rt0Address.unit;
            rtAddresses.rt0Page = rt0Address.page;
            rtAddresses.rt1Unit = rt1Address.unit;
            rtAddresses.rt1Page = rt1Address.page;
            rtAddresses.rt2Unit = rt2Address.unit;
            rtAddresses.rt2Page = rt2Address.page;
            //rtAddresses.rt3Unit = rt3Address.unit;
            //rtAddresses.rt3Page = rt3Address.page;
            rtAddresses.depthUnit = rtDepthAddress.unit;
            rtAddresses.depthPage = rtDepthAddress.page;

            rtsBuffer->data(
                sizeof(renderTargetsAddresses),
                &rtAddresses,
                bufferDataUsage::dynamicDraw);
        });

        pass->setOnBeginRender([=](phi::program* program, framebuffer* framebuffer)
        {
            framebuffer->bindForDrawing();
            glDisable(GL_DEPTH_TEST);
            glClear(GL_COLOR_BUFFER_BIT);

            program->bind();

            if (texturesManager::getIsBindless())//TODO: AAAAAAAAAAAAA ARRUMA LAMBDAS SOMETHING
                program->setUniform(0, texturesManager::handles);
            else
                program->setUniform(0, texturesManager::units);
        });

        pass->setOnRender([=](const vector<vertexArrayObject*>& vaos)
        {
            for (auto vao : vaos)
                vao->render();
        });

        pass->setOnEndRender([=](phi::program* program, framebuffer* framebuffer)
        {
            program->unbind();
            glEnable(GL_DEPTH_TEST);

            //framebuffer->blitToDefault(finalImageRT);

            //glActiveTexture(GL_TEXTURE0 + finalImageRT->textureAddress.unit);
            //glBindTexture(GL_TEXTURE_2D_ARRAY, finalImageRT->textureAddress.containerId);
            //glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
        });

        return pass;
    }
}
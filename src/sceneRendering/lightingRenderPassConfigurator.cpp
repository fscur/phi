#include <precompiled.h>
#include "lightingRenderPassConfigurator.h"

#include <core\notImplementedException.h>
#include <core\geometry.h>

#include <rendering\texturesManager.h>
#include <rendering\renderTargetsAddresses.h>
#include <rendering\textureAddress.h>
#include <rendering\vertexArrayObject.h>

#include <rendering\programBuilder.h>

namespace phi
{
    renderPass* lightingRenderPassConfigurator::configureNewLighting(renderPass* gBufferRenderPass, resolution resolution, string& shadersPath)
    {
        auto renderTargets = gBufferRenderPass->getOuts();

        auto rtAddresses = phi::renderTargetsAddresses();
        auto rt0 = renderTargets["rt0"]->textureAddress;
        auto rt1 = renderTargets["rt1"]->textureAddress;
        auto rt2 = renderTargets["rt2"]->textureAddress;
        auto rt3 = renderTargets["pickingRenderTarget"]->textureAddress;
        auto depth = renderTargets["depth"]->textureAddress;

        rtAddresses.rt0Unit = rt0.unit;
        rtAddresses.rt0Page = rt0.page;
        rtAddresses.rt1Unit = rt1.unit;
        rtAddresses.rt1Page = rt1.page;
        rtAddresses.rt2Unit = rt2.unit;
        rtAddresses.rt2Page = rt2.page;
        rtAddresses.rt3Unit = rt3.unit;
        rtAddresses.rt3Page = rt3.page;
        rtAddresses.depthUnit = depth.unit;
        rtAddresses.depthPage = depth.page;

        auto layout = textureLayout();
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

        auto finalImageTexAddress = texturesManager::get(finalImageTexture);

        auto finalImageRT = new renderTarget(
            "finalImageRenderTarget",
            static_cast<GLint>(resolution.width),
            static_cast<GLint>(resolution.height),
            finalImageTexAddress);

        auto finalImageFramebuffer = new framebuffer();
        finalImageFramebuffer->add(GL_COLOR_ATTACHMENT0, finalImageRT);

        auto quad = geometry::createQuad(2.0f);
        vector<vertexAttrib> attribs;
        attribs.push_back(vertexAttrib(0, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::position)));
        attribs.push_back(vertexAttrib(1, 2, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::texCoord)));

        auto vbo = new vertexBuffer("vbo", attribs);
        vbo->storage(quad->vboSize, quad->vboData, bufferStorageUsage::write);

        auto ebo = new buffer("ebo", bufferTarget::element);
        ebo->storage(quad->eboSize, quad->eboData, bufferStorageUsage::write);

        auto quadVao = new vertexArrayObject();
        quadVao->add(vbo);
        quadVao->setEbo(ebo);
        quadVao->setOnRender([]
        {
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            
        });

        auto rtsBuffer = new buffer("RenderTargetAddresses", bufferTarget::uniform);

        rtsBuffer->storage(
            sizeof(renderTargetsAddresses),
            &rtAddresses,
            bufferStorageUsage::write);

        auto program = programBuilder::buildProgram(shadersPath, "lighting", "lighting");
        program->addBuffer(rtsBuffer);

        auto pass = new renderPass(program, finalImageFramebuffer);
        pass->addOut(finalImageRT);
        pass->addVao(quadVao);

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

            framebuffer->blitToDefault(finalImageRT);

            glActiveTexture(GL_TEXTURE0 + finalImageRT->textureAddress.unit);
            glBindTexture(GL_TEXTURE_2D_ARRAY, finalImageRT->textureAddress.containerId);
            glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
        });

        return pass;
    }
}
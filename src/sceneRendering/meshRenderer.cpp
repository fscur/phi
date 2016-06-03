#include <precompiled.h>
#include "meshRenderer.h"

#include <core\notImplementedException.h>
#include <core\node.h>

#include <io\path.h>

#include <rendering\framebufferBuilder.h>
#include <rendering\programBuilder.h>
#include <rendering\vao.h>
#include <rendering\postProcessVao.h>

#include "gBufferRenderPassConfigurator.h"
#include "lightingRenderPassConfigurator.h"

namespace phi
{
    meshRenderer::meshRenderer(vector<renderPass*>&& renderPasses) :
        _renderPasses(renderPasses)
    {
    }

    meshRenderer::~meshRenderer()
    {
        for (auto renderPass : _renderPasses)
            safeDelete(renderPass);
    }

    void meshRenderer::render()
    {
        for (auto renderPass : _renderPasses)
            renderPass->render();
    }

    meshRenderer* meshRenderer::configure(gl* gl, float width, float height, const string& resourcesPath, meshRendererDescriptor* rendererDescriptor)
    {
        auto shadersPath = path::combine(resourcesPath, "/shaders");

        auto gBufferFramebuffer = framebufferBuilder::newFramebuffer(gl, width, height)
            .with(GL_COLOR_ATTACHMENT0, GL_RGBA16F, GL_RGBA)
            .with(GL_COLOR_ATTACHMENT1, GL_RGBA16F, GL_RGBA)
            .with(GL_COLOR_ATTACHMENT2, GL_RGBA16F, GL_RGBA)
            .with(GL_COLOR_ATTACHMENT3, GL_RGBA8, GL_RGBA)
            .with(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT)
            .build();

        auto renderTargets = gBufferFramebuffer->getRenderTargets();

        auto rtAddresses = new phi::renderTargetsAddresses();
        rtAddresses->rt0Unit = renderTargets[0]->textureAddress.unit;
        rtAddresses->rt1Unit = renderTargets[1]->textureAddress.unit;
        rtAddresses->rt2Unit = renderTargets[2]->textureAddress.unit;
        rtAddresses->rt3Unit = renderTargets[3]->textureAddress.unit;
        rtAddresses->depthUnit = renderTargets[4]->textureAddress.unit;
        rtAddresses->rt0Page = renderTargets[0]->textureAddress.page;
        rtAddresses->rt1Page = renderTargets[1]->textureAddress.page;
        rtAddresses->rt2Page = renderTargets[2]->textureAddress.page;
        rtAddresses->rt3Page = renderTargets[3]->textureAddress.page;
        rtAddresses->depthPage = renderTargets[4]->textureAddress.page;

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

        auto rtsBuffer = new buffer<renderTargetsAddresses>(bufferTarget::uniform);

        rtsBuffer->storage(
            sizeof(renderTargetsAddresses),
            &*rtAddresses,
            bufferStorageUsage::write);
        rtsBuffer->bindBufferBase(2);

        auto gBufferProgram = programBuilder::buildProgram(shadersPath, "gBuffer", "gBuffer");
        auto gBuffer = new renderPass(gBufferProgram);
        gBuffer->setOnRender([=](program* program)
        {
            rendererDescriptor->bind();
            gBufferFramebuffer->bindForDrawing();

            const auto selectionRenderTargetNumber = 3;
            auto selectionClearColor = 0.0f;

            glDepthMask(GL_TRUE);
            glError::check();
            glEnable(GL_DEPTH_TEST);
            glError::check();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glError::check();
            glClearBufferfv(GL_COLOR, selectionRenderTargetNumber, &selectionClearColor);
            glError::check();

            program->bind();

            if (gl->currentState.useBindlessTextures)
                program->setUniform(0, gl->texturesManager->handles);
            else
                program->setUniform(0, gl->texturesManager->units);

            for (auto batch : rendererDescriptor->_batches)
                batch->render();

            program->unbind();

            gBufferFramebuffer->unbind(GL_FRAMEBUFFER);

            gBufferFramebuffer->bindForReading();
            auto w = static_cast<GLint>(width);
            auto h = static_cast<GLint>(height);

            glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
            glError::check();
        });

        auto lightingProgram = programBuilder::buildProgram(shadersPath, "lighting", "lighting");
        auto lighting = new renderPass(lightingProgram);
        lighting->setOnRender([=](program* program)
        {
            finalImageFramebuffer->bindForDrawing();

            glDisable(GL_DEPTH_TEST);
            glClear(GL_COLOR_BUFFER_BIT);

            quadVao->bind();

            program->bind();

            if (gl->currentState.useBindlessTextures)
                program->setUniform(0, gl->texturesManager->handles);
            else
                program->setUniform(0, gl->texturesManager->units);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glError::check();
            program->unbind();

            glBindVertexArray(0);
            glError::check();

            glEnable(GL_DEPTH_TEST);

            finalImageFramebuffer->blitToDefault(finalImageRT);
        });

        //auto gBufferRenderPass = gBufferRenderPassConfigurator::configureNewGBuffer();
        //auto lightingRenderPass = lightingRenderPassConfigurator::configureNewLighting(gBufferRenderPass->getOuts());

        /*return new meshRenderer({ gBufferRenderPass, lightingRenderPass });*/
        return new meshRenderer({ gBuffer, lighting });
    }
}
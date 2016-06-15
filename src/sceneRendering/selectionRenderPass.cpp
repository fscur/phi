#include <precompiled.h>
#include "selectionRenderPass.h"

#include <core\time.h>
#include <core\geometry.h>

#include <rendering\texturesManager.h>
#include <rendering\renderTargetsAddresses.h>
#include <rendering\textureAddress.h>
#include <rendering\vertexArrayObject.h>

#include <rendering\programBuilder.h>

namespace phi
{
    renderPass* selectionRenderPass::configure(const renderPass* lightingRenderPass, const resolution& resolution, const string& shadersPath)
    {
        //auto renderTargets = lightingRenderPass->getOuts();

        //auto rtAddresses = phi::renderTargetsAddresses();
        //auto rt0 = renderTargets["rt0"]->textureAddress;
        //auto rt1 = renderTargets["rt1"]->textureAddress;
        //auto rt2 = renderTargets["rt2"]->textureAddress;
        //auto rt3 = renderTargets["pickingRenderTarget"]->textureAddress;
        //auto depth = renderTargets["depth"]->textureAddress;

        //rtAddresses.rt0Unit = rt0.unit;
        //rtAddresses.rt0Page = rt0.page;
        //rtAddresses.rt1Unit = rt1.unit;
        //rtAddresses.rt1Page = rt1.page;
        //rtAddresses.rt2Unit = rt2.unit;
        //rtAddresses.rt2Page = rt2.page;
        //rtAddresses.rt3Unit = rt3.unit;
        //rtAddresses.rt3Page = rt3.page;
        //rtAddresses.depthUnit = depth.unit;
        //rtAddresses.depthPage = depth.page;

        //auto finalImageFramebuffer = new framebuffer();
        //auto finalImageRenderTarget = renderTargets["finalImageRenderTarget"];
        //finalImageFramebuffer->add(GL_COLOR_ATTACHMENT0, finalImageRenderTarget);

        //auto quadVao = vertexArrayObject::createPostProcessVao();
        //auto rtsBuffer = new buffer("RenderTargetAddresses", bufferTarget::uniform);

        //rtsBuffer->storage(
        //    sizeof(renderTargetsAddresses),
        //    &rtAddresses,
        //    bufferStorageUsage::write);

        //auto program = programBuilder::buildProgram(shadersPath, "selection", "selection");
        //program->addBuffer(rtsBuffer);

        //auto pass = new renderPass(program, finalImageFramebuffer);
        //pass->addOut(finalImageRenderTarget);
        //pass->addVao(quadVao);

        //pass->setOnBeginRender([=](phi::program* program, framebuffer* framebuffer)
        //{
        //    framebuffer->bindForDrawing();

        //    glEnable(GL_BLEND);
        //    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //    program->bind();

        //    program->setUniform(0, resolution.toVec2());
        //    program->setUniform(1, 0.5f);
        //    program->setUniform(2, static_cast<float>(time::totalSeconds));

        //    if (texturesManager::getIsBindless())//TODO: AAAAAAAAAAAAA ARRUMA LAMBDAS SOMETHING
        //        program->setUniform(3, texturesManager::handles);
        //    else
        //        program->setUniform(3, texturesManager::units);
        //});

        //pass->setOnRender([=](const vector<vertexArrayObject*>& vaos)
        //{
        //    for (auto vao : vaos)
        //        vao->render();
        //});

        //pass->setOnEndRender([=](phi::program* program, framebuffer* framebuffer)
        //{
        //    program->unbind();

        //    glDisable(GL_BLEND);

        //    framebuffer->blitToDefault(finalImageRenderTarget);
        //    framebuffer->unbind(GL_FRAMEBUFFER);

        //    //auto w = static_cast<GLint>(resolution.width);
        //    //auto h = static_cast<GLint>(resolution.height);
        //    //auto picking = framebuffer::getPickingFramebuffer();
        //    //auto rt = framebuffer::getPickingRenderTarget();
        //    //picking->bindForReading(rt);
        //    //glBlitFramebuffer(0, 0, w, h, 0, 0, (GLint)(256 * resolution.getAspect()), 256, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        //    glActiveTexture(GL_TEXTURE0 + finalImageRenderTarget->textureAddress.unit);
        //    glBindTexture(GL_TEXTURE_2D_ARRAY, finalImageRenderTarget->textureAddress.containerId);
        //    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
        //});

        //return pass;

        throw exception("cú preto");
    }
}
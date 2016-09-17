#include <precompiled.h>
#include "obbBoxRenderPass.h"

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
    renderPass* obbBoxRenderPass::configure(
        const obbRenderAdapter* renderAdapter,
        const resolution& resolution,
        const string& shadersPath,
        framebufferAllocator* framebufferAllocator)
    {
        auto defaultFramebuffer = framebufferAllocator->getFramebuffer("defaultFramebuffer");

        auto boxColliderProgram = programBuilder::buildProgram(shadersPath, "obbBox", "obbBox");

        auto pass = new renderPass(boxColliderProgram, defaultFramebuffer, resolution);
        pass->addVao(renderAdapter->getVao());

        pass->setOnBeginRender([=](program* program, framebuffer* framebuffer, const phi::resolution& resolution)
        {
            glDisable(GL_CULL_FACE);
            glDepthMask(GL_FALSE);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(-1.0f, 0.0f);
            framebuffer->bindForDrawing();

            program->bind();
            program->setUniform(0, vec4(0.0, 0.0, 1.0, 1.0));
        });

        pass->setOnRender([=](const vector<vertexArrayObject*>& vaos)
        {
            for (auto vao : vaos)
                vao->render();
        });

        pass->setOnEndRender([=](phi::program* program, framebuffer* framebuffer, const phi::resolution& resolution)
        {
            framebuffer->unbind(GL_FRAMEBUFFER);
            program->unbind();

            glPolygonOffset(0.0f, 0.0f);
            glDisable(GL_POLYGON_OFFSET_FILL);
            glDisable(GL_BLEND);
            glDepthMask(GL_TRUE);
            glEnable(GL_CULL_FACE);
            glDisable(GL_DEPTH_TEST);
        });

        return pass;
    }
}
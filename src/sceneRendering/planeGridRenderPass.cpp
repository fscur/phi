#include <precompiled.h>
#include "planeGridRenderPass.h"

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
    renderPass* planeGridRenderPass::configure(
        const planeGridRenderAdapter* renderAdapter,
        const resolution& resolution,
        const string& shadersPath,
        framebufferAllocator* framebufferAllocator)
    {
        auto defaultFramebuffer = framebufferAllocator->getFramebuffer("defaultFramebuffer");
        auto defaultRenderTarget = defaultFramebuffer->getRenderTarget("defaultRenderTarget");

        auto planeGridProgram = programBuilder::buildProgram(shadersPath, "planeGrid", "planeGrid");
        planeGridProgram->addBuffer(renderAdapter->getMousePlaneGridRenderDataBuffer());

        auto pass = new renderPass(planeGridProgram, defaultFramebuffer, resolution);
        pass->addVao(renderAdapter->getVao());

        pass->setOnCanRender([=]()
        {
            return true;
        });

        pass->setOnBeginRender([=](program* program, framebuffer* framebuffer, const phi::resolution& resolution)
        {
            framebuffer->bindForDrawing();
            glDisable(GL_CULL_FACE);
            glDepthMask(GL_FALSE);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(-1.0f, 0.0f);
            glEnable(GL_CLIP_DISTANCE0);
            glEnable(GL_CLIP_DISTANCE1);
            glEnable(GL_CLIP_DISTANCE2);

            program->bind();
        });

        pass->setOnRender([=](const vector<vertexArrayObject*>& vaos)
        {
            for (auto vao : vaos)
                vao->render();
        });

        pass->setOnEndRender([=](phi::program* program, framebuffer* framebuffer, const phi::resolution& resolution)
        {
            program->unbind();

            glDisable(GL_CLIP_DISTANCE2);
            glDisable(GL_CLIP_DISTANCE1);
            glDisable(GL_CLIP_DISTANCE0);

            glPolygonOffset(0.0f, 0.0f);
            glDisable(GL_POLYGON_OFFSET_FILL);
            glDisable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
            glEnable(GL_CULL_FACE);

            auto w = static_cast<GLint>(resolution.width);
            auto h = static_cast<GLint>(resolution.height);

            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

            framebuffer->bindForReading(defaultRenderTarget);

            glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_LINEAR);

            auto address = texturesManager::getTextureAddress(defaultRenderTarget->texture);
            glActiveTexture(GL_TEXTURE0 + address.unit);
            glBindTexture(GL_TEXTURE_2D_ARRAY, address.arrayId);
            glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
        });

        return pass;
    }
}
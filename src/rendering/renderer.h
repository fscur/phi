#pragma once

#include "pipeline.h"
#include "batch.h"
#include "gBufferRenderPass.h"
#include "lightingRenderPass.h"

namespace phi
{
    class renderer
    {
    private:
        phi::gl* _gl;

    public:
        size_t w;
        size_t h;
        phi::gBufferRenderPass* gBufferPass;
        phi::lightingRenderPass* lightingPass;
        framebuffer* defaultFramebuffer;

        RENDERING_API renderer(phi::gl* gl, size_t w, size_t h) :
            _gl(gl),
            w(w),
            h(h)
        {
            defaultFramebuffer = new framebuffer(true);
            gBufferPass = new phi::gBufferRenderPass(gl, w, h);
            lightingPass = new phi::lightingRenderPass(gBufferPass, gl, w, h);
        }

        RENDERING_API ~renderer() {}

        RENDERING_API  void renderer::render()
        {
            gBufferPass->render();
            lightingPass->render();
            
            /*
            gBufferPass->framebuffer->bindForReading(gBufferPass->targets[0]);
            glBlitFramebuffer(0, 0, w, h, 0, 192 * 0, 256, 192, GL_COLOR_BUFFER_BIT, GL_LINEAR);

            gBufferPass->framebuffer->bindForReading(gBufferPass->targets[1]);
            glBlitFramebuffer(0, 0, w, h, 0, 192 * 1, 256, 192 * 2, GL_COLOR_BUFFER_BIT, GL_LINEAR);

            gBufferPass->framebuffer->bindForReading(gBufferPass->targets[2]);
            glBlitFramebuffer(0, 0, w, h, 0, 192 * 2, 256, 192 * 3, GL_COLOR_BUFFER_BIT, GL_LINEAR);
            */
        }

        RENDERING_API void update()
        {
        }
    };
}
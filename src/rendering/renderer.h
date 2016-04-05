#pragma once
#include <phi.h>
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

        renderer(phi::gl* gl, size_t w, size_t h) :
            _gl(gl),
            w(w),
            h(h)
        {
            defaultFramebuffer = new framebuffer(true);
            gBufferPass = new phi::gBufferRenderPass(gl, w, h);
            lightingPass = new phi::lightingRenderPass(gBufferPass, gl, w, h);
        }

        ~renderer() 
        {
            safeDelete(defaultFramebuffer);
            safeDelete(gBufferPass);
            safeDelete(lightingPass);
        }

        void render()
        {
            gBufferPass->render();
            lightingPass->render();
        }

        void update()
        {
            gBufferPass->update();
            lightingPass->update();
        }
    };
}
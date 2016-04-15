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
        gl* _gl;
    public:
        gBufferRenderPass* gBufferPass;
        lightingRenderPass* lightingPass;
        framebuffer* defaultFramebuffer;
        size_t w;
        size_t h;

        renderer(phi::gl* gl, size_t w, size_t h) :
            _gl(gl),
            gBufferPass(new gBufferRenderPass(gl, w, h)),
            lightingPass(new lightingRenderPass(gBufferPass, gl, w, h)),
            defaultFramebuffer(new framebuffer(true)),
            w(w),
            h(h)
        {
        }

        ~renderer()
        {
            safeDelete(gBufferPass);
            safeDelete(lightingPass);
            safeDelete(defaultFramebuffer);
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
#pragma once
#include <phi.h>
#include "pipeline.h"
#include "batch.h"
#include "gBufferRenderPass.h"
#include "lightingRenderPass.h"
#include "planeGridPass.h"

namespace phi
{
    class renderer
    {
    private:
        gl* _gl;
    public:
        size_t w;
        size_t h;
        gBufferRenderPass* gBufferPass;
        lightingRenderPass* lightingPass;
        planeGridPass* planeGridPass;
        framebuffer* defaultFramebuffer;

        renderer(phi::gl* gl, size_t w, size_t h) :
            _gl(gl),
            w(w),
            h(h)
        {
            defaultFramebuffer = new framebuffer(true);
            gBufferPass = new gBufferRenderPass(gl, w, h);
            lightingPass = new lightingRenderPass(gBufferPass, gl, w, h);
            planeGridPass = new phi::planeGridPass(gl, w, h);
        }

        ~renderer() 
        {
            safeDelete(defaultFramebuffer);
            safeDelete(gBufferPass);
            safeDelete(lightingPass);
            safeDelete(planeGridPass);
        }

        void render()
        {
            gBufferPass->render();
            lightingPass->render();
            planeGridPass->render();
        }

        void update()
        {
            gBufferPass->update();
            lightingPass->update();
            planeGridPass->update();
        }
    };
}
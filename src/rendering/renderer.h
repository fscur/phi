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
        float w;
        float h;

        renderer(gl* gl, float w, float h) :
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
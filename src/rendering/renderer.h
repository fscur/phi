#pragma once
#include <phi.h>
#include "pipeline.h"
#include "batch.h"
#include "gBufferRenderPass.h"
#include "lightingRenderPass.h"
#include "selectionRenderPass.h"
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
        selectionRenderPass* selectionPass;
        planeGridPass* planeGridPass;
        framebuffer* defaultFramebuffer;

        renderer(phi::gl* gl, size_t w, size_t h) :
            _gl(gl),
            w(w),
            h(h),
            gBufferPass(new gBufferRenderPass(gl, w, h)),
            lightingPass(new lightingRenderPass(gBufferPass, gl, w, h)),
            selectionPass(new selectionRenderPass(gBufferPass, gl, w, h)),
            planeGridPass(new phi::planeGridPass(gl, w, h)),
            defaultFramebuffer(new framebuffer(true))
        {
        }

        ~renderer()
        {
            safeDelete(defaultFramebuffer);
            safeDelete(gBufferPass);
            safeDelete(lightingPass);
            safeDelete(selectionPass);
            safeDelete(planeGridPass);
        }

        framebuffer* getDefaultFramebuffer() const { return _defaultFramebuffer; }
        planeGridRenderPass* getPlaneGridRenderPass() const { return _planeGridRenderPass; }
        gBufferRenderPass* getGBufferRenderPass() const { return _gBufferRenderPass; }
		renderTarget* getFinalImageRT() const { return _finalImageRT; }

        RENDERING_API void render();
        RENDERING_API void update();
    };
}
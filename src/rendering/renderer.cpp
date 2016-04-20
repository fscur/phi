#include <precompiled.h>
#include "renderer.h"

namespace phi
{
    inline renderer::renderer(gl * gl, float w, float h) :
        _gl(gl),
        _defaultFramebuffer(new framebuffer(true)),
        _gBufferRenderPass(new gBufferRenderPass(gl, w, h)),
        _lightingRenderPass(new lightingRenderPass(_gBufferRenderPass, gl, w, h)),
        _planeGridRenderPass(new planeGridRenderPass(gl, w, h)),
        _w(w),
        _h(h)
    {
    }
    inline renderer::~renderer()
    {
        safeDelete(_defaultFramebuffer);
        safeDelete(_gBufferRenderPass);
        safeDelete(_lightingRenderPass);
        safeDelete(_planeGridRenderPass);
    }
    inline void renderer::render()
    {
        _gBufferRenderPass->render();
        _lightingRenderPass->render();
        _planeGridRenderPass->render();
    }
    inline void renderer::update()
    {
        _gBufferRenderPass->update();
        _lightingRenderPass->update();
        _planeGridRenderPass->update();
    }
}
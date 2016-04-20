#pragma once
#include <phi.h>
#include "renderingApi.h"
#include "pipeline.h"
#include "batch.h"
#include "gBufferRenderPass.h"
#include "lightingRenderPass.h"
#include "planeGridRenderPass.h"

namespace phi
{
    class renderer
    {
    private:
        gl* _gl;
        framebuffer* _defaultFramebuffer;
        gBufferRenderPass* _gBufferRenderPass;
        lightingRenderPass* _lightingRenderPass;
        planeGridRenderPass* _planeGridRenderPass;
        float _w;
        float _h;

    public:
        RENDERING_API renderer(gl* gl, float w, float h);
        RENDERING_API ~renderer();

        framebuffer* getDefaultFramebuffer() const { return _defaultFramebuffer; }
        planeGridRenderPass* getPlaneGridRenderPass() const { return _planeGridRenderPass; }
        gBufferRenderPass* getGBufferRenderPass() const { return _gBufferRenderPass; }

        RENDERING_API void render();
        RENDERING_API void update();
    };
}
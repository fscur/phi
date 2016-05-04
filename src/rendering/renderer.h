#pragma once
#include <phi.h>
#include "pipeline.h"
#include "batch.h"
#include "gBufferRenderPass.h"
#include "lightingRenderPass.h"
#include "selectionRenderPass.h"
#include "planeGridRenderPass.h"

namespace phi
{
    class renderer
    {
    private:
        gl* _gl;
        gBufferRenderPass* _gBufferRenderPass;
        lightingRenderPass* _lightingRenderPass;
        selectionRenderPass* _selectionRenderPass;
        planeGridRenderPass* _planeGridRenderPass;
        framebuffer* _defaultFramebuffer;
        framebuffer* _finalImageFramebuffer;
        renderTarget* _finalImageRT;
        float _w;
        float _h;
    public:

        RENDERING_API renderer(gl * gl, float w, float h);
        RENDERING_API ~renderer();

        RENDERING_API void render();
        RENDERING_API void update();

        framebuffer* getDefaultFramebuffer() const { return _defaultFramebuffer; }
        planeGridRenderPass* getPlaneGridRenderPass() const { return _planeGridRenderPass; }
        gBufferRenderPass* getGBufferRenderPass() const { return _gBufferRenderPass; }
        renderTarget* getFinalImageRT() const { return _finalImageRT; }
    };
}
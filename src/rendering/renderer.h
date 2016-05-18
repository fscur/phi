#pragma once
#include <phi.h>

#include "batch.h"
#include "renderTargetsAddresses.h"
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
        framebuffer* _gBufferFramebuffer;
        framebuffer* _finalImageFramebuffer;
        renderTarget* _finalImageRT;

        renderTargetsAddresses _rtAddresses;
        float _w;
        float _h;

    public:
        renderTarget* rt0;
        renderTarget* rt1;
        renderTarget* rt2;
        renderTarget* rt3;
        renderTarget* depth;

    private:
        void createRenderTargets();
        void blitGBufferDepthToDefaultFrameBuffer();
        void clearGBufferFrameBuffer();
        void generateFinalImageMipMaps();

    public:
        RENDERING_API renderer(gl* gl, float w, float h);
        RENDERING_API ~renderer();

        RENDERING_API void render(const vector<batch*>& batches);
        RENDERING_API void update();

        framebuffer* getDefaultFramebuffer() const { return _defaultFramebuffer; }
        framebuffer* getGBufferFramebuffer() const { return _gBufferFramebuffer; }
        planeGridRenderPass* getPlaneGridRenderPass() const { return _planeGridRenderPass; }
        gBufferRenderPass* getGBufferRenderPass() const { return _gBufferRenderPass; }
        renderTarget* getFinalImageRT() const { return _finalImageRT; }
    };
}
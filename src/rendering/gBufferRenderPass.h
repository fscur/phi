#pragma once
#include <phi.h>
#include "renderingApi.h"
#include "gl.h"
#include "batch.h"
#include "shader.h"
#include "renderTarget.h"
#include "framebuffer.h"
#include "batch.h"

namespace phi
{
    class gBufferRenderPass
    {
    private:
        gl* _gl;
        float _w;
        float _h;
        vector<batch*> _batches;
        shader* _shader;
        framebuffer* _framebuffer;
    public:
        renderTarget* rt0;
        renderTarget* rt1;
        renderTarget* rt2;
        renderTarget* rt3;
        renderTarget* depth;
    private:
        void initShader();
        void initRenderTargets();

    public:
        RENDERING_API gBufferRenderPass(gl* gl, float w, float h);
        RENDERING_API ~gBufferRenderPass();

        RENDERING_API void update();
        RENDERING_API void render();

        void setBatches(vector<batch*> value) { _batches = value; }
        framebuffer* getFramebuffer() { return _framebuffer; }
    };
}
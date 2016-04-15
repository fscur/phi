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
        size_t _w;
        size_t _h;
    public:
        shader* shader;
        framebuffer* framebuffer;
        vector<renderTarget*> targets;
        vector<batch*> batches;
    private:
        void initShader();
        void initRenderTargets();
    public:
        RENDERING_API gBufferRenderPass(gl* gl, size_t w, size_t h);
        RENDERING_API ~gBufferRenderPass();

        RENDERING_API void update();
        RENDERING_API void render();
    };
}
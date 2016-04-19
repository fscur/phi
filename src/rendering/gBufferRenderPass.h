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
    public:
        shader* shader;
        framebuffer* framebuffer;
        vector<renderTarget*> targets;
        vector<batch*> batches;
    private:
        void initShader();
        void initRenderTargets();
    public:
        RENDERING_API gBufferRenderPass(gl* gl, float w, float h);
        RENDERING_API ~gBufferRenderPass();

        RENDERING_API void update();
        RENDERING_API void render();
    };
}
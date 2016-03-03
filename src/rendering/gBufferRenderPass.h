#pragma once
#include <phi.h>
#include "rendering.h"
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
        phi::gl* _gl;
        size_t _w;
        size_t _h;

    public:
        phi::shader* shader;
        vector<renderTarget*> targets;
        vector<phi::batch*> batches;
        phi::framebuffer* framebuffer;

    private:
        void initShader();
        void initRenderTargets();

    public:
        RENDERING_API gBufferRenderPass(phi::gl* gl, size_t w, size_t h);
        RENDERING_API ~gBufferRenderPass();

        RENDERING_API void update();
        RENDERING_API void render();
    };
}
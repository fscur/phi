#pragma once

#include "rendering.h"
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
        std::vector<renderTarget*> targets;
        std::vector<phi::batch*> batches;
        phi::framebuffer* framebuffer;

    private:
        void initShader();
        void initRenderTargets();

    public:
        gBufferRenderPass(phi::gl* gl, size_t w, size_t h);
        ~gBufferRenderPass();

        void update();
        void render();
    };
}
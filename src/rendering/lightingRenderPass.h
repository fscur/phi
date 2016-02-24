#pragma once

#include "gBufferRenderPass.h"
#include "framebuffer.h"
#include "batch.h"

namespace phi
{
    struct rtAddresses
    {
        int rt0Unit;
        int rt1Unit;
        int rt2Unit;
        int rt3Unit;
        float rt0Page;
        float rt1Page;
        float rt2Page;
        float rt3Page;
    };

    class lightingRenderPass
    {
    private:
        phi::gl* _gl;
        size_t _w;
        size_t _h;
        geometry* _quad;
        GLuint _quadVao;
        vertexBuffer* _quadVbo;
        buffer* _quadEbo;
        buffer* _rtsBuffer;

    private:
        void createQuad();
        void renderQuad();

    public:
        shader* shader;
        std::vector<phi::batch*> batches;

    public:
        lightingRenderPass(phi::gBufferRenderPass* gBufferPass, phi::gl* gl, size_t w, size_t h);

        void update();
        void render();
    };
}
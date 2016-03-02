#pragma once
#include <precompiled.h>
#include "rendering.h"
#include "gl.h"
#include "gBufferRenderPass.h"
#include "vertexBuffer.h"
#include "buffer.h"
#include "shader.h"
#include "batch.h"
#include <core\geometry.h>

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
        uint _quadVao;
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
        RENDERING_API lightingRenderPass(phi::gBufferRenderPass* gBufferPass, phi::gl* gl, size_t w, size_t h);

        RENDERING_API void update();
        RENDERING_API void render();
    };
}
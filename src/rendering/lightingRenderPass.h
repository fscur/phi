#pragma once
#include <phi.h>
#include "renderingApi.h"
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
        gl* _gl;
        geometry* _quad;
        vertexBuffer* _quadVbo;
        buffer* _quadEbo;
        buffer* _rtsBuffer;
        size_t _w;
        size_t _h;
        uint _quadVao;

    private:
        void createQuad();
        void renderQuad();

    public:
        shader* shader;

    public:
        RENDERING_API lightingRenderPass(gBufferRenderPass* gBufferPass, gl* gl, size_t w, size_t h);
        RENDERING_API ~lightingRenderPass();

        RENDERING_API void update();
        RENDERING_API void render();
    };
}
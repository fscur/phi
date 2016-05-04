#pragma once
#include <phi.h>
#include "renderingApi.h"
#include "shader.h"
#include "gl.h"
#include "gBufferRenderPass.h"

namespace phi
{
    class selectionRenderPass
    {
    private:
        const gl* _gl;
        shader* _shader;
        geometry* _quad;
        vertexBuffer* _quadVbo;
        buffer* _quadEbo;
        buffer* _rtsBuffer;
        uint _quadVao;
        float _w;
        float _h;
    private:
        void createQuad();

    public:
        RENDERING_API selectionRenderPass(const gBufferRenderPass* const gBufferPass, const gl* const gl, float w, float h);
        RENDERING_API ~selectionRenderPass();

        RENDERING_API void update();
        RENDERING_API void render();
    };
}
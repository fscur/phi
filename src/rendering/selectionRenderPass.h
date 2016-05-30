#pragma once
#include <phi.h>
#include "renderingApi.h"
#include "program.h"
#include "gl.h"
#include "gBufferRenderPass.h"
#include "renderTargetsAddresses.h"

namespace phi
{
    class selectionRenderPass
    {
    private:
        const gl* _gl;
        program* _program;
        geometry* _quad;
        vertexBuffer<vertex>* _quadVbo;
        buffer<uint>* _quadEbo;
        buffer<renderTargetsAddresses>* _rtsBuffer;
        uint _quadVao;
        float _w;
        float _h;
    private:
        void createQuad();

    public:
        RENDERING_API selectionRenderPass(renderTargetsAddresses& rtAddresses, const gl* const gl, float w, float h);
        RENDERING_API ~selectionRenderPass();

        RENDERING_API void update();
        RENDERING_API void render();
    };
}
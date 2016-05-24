#pragma once
#include <phi.h>
#include "renderingApi.h"
#include "gl.h"
#include "renderTargetsAddresses.h"
#include "vertexBuffer.h"
#include "buffer.h"
#include "shader.h"
#include "batch.h"

#include <core\geometry.h>

namespace phi
{
    class lightingRenderPass
    {
    private:
        gl* _gl;
        geometry* _quad;
        shader* _shader;
        vertexBuffer* _quadVbo;
        buffer* _quadEbo;
        buffer* _rtsBuffer;
        float _w;
        float _h;
        uint _quadVao;

    private:
        void createQuad();

    public:
        RENDERING_API lightingRenderPass(const renderTargetsAddresses& rtAddresses, gl* gl, float w, float h);
        RENDERING_API ~lightingRenderPass();

        RENDERING_API void update();
        RENDERING_API void render();
    };
}
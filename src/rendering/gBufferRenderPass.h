#pragma once
#include <phi.h>
#include "renderingApi.h"
#include "gl.h"
#include "batch.h"
#include "program.h"
#include "renderTarget.h"
#include "framebuffer.h"
#include "batch.h"

namespace phi
{
    class gBufferRenderPass
    {
    private:
        gl* _gl;
        program* _program;

    private:
        void initShader();

    public:
        RENDERING_API gBufferRenderPass(gl* gl);
        RENDERING_API ~gBufferRenderPass();

        RENDERING_API void update();
        RENDERING_API void render(const vector<batch*>& batches);
    };
}
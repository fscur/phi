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
        shader* _shader;
        framebuffer* _framebuffer;
        vector<renderTarget*> _targets;
        vector<batch*> _batches;
        float _w;
        float _h;

    private:
        void initShader();
        void initRenderTargets();

    public:
        RENDERING_API gBufferRenderPass(gl* gl, float w, float h);
        RENDERING_API ~gBufferRenderPass();

        RENDERING_API void update();
        RENDERING_API void render();

        vector<renderTarget*> getTargets() const { return _targets; }
        void setBatches(vector<batch*> value) { _batches = value; }
    };
}
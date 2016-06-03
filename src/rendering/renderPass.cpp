#include <precompiled.h>
#include "renderPass.h"

namespace phi
{
    renderPass::renderPass(program* program, framebuffer* framebuffer) :
        _program(program),
        _framebuffer(framebuffer)
    {
    }

    renderPass::~renderPass()
    {
    }

    void renderPass::update()
    {
        //_onUpdate(_program);
    }

    void renderPass::render()
    {
        _framebuffer->bindForDrawing();

        auto i = 1u;
        for (auto buffer : _buffers)
            buffer->bindBufferBase(i++);

        _onBeginRender(_program, _framebuffer);
        _onRender(_vaos);
        _onEndRender(_program, _framebuffer);
    }
}

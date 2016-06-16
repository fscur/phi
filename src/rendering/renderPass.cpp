#include <precompiled.h>
#include "renderPass.h"

namespace phi
{
    renderPass::renderPass(program* program, framebuffer* framebuffer, const resolution& resolution) :
        _program(program),
        _framebuffer(framebuffer),
        _resolution(resolution)
    {
    }

    renderPass::~renderPass()
    {
    }

    void renderPass::initialize()
    {
        if (_onInitialize)
            _onInitialize();
    }

    void renderPass::update()
    {
        //_onUpdate(_program);
    }

    void renderPass::render()
    {
        _framebuffer->bindForDrawing();

        //TODO: frameUniformBlocks are buffer 0 
        //auto i = 1u;
        //for (auto buffer : _buffers)
            //buffer->bindBufferBase(i++);

        _onBeginRender(_program, _framebuffer, _resolution);
        _onRender(_vaos);
        _onEndRender(_program, _framebuffer);
    }

    void renderPass::resize(const resolution& resolution)
    {
        _resolution = resolution;

        if (_onResize)
            _onResize(resolution);
    }
}

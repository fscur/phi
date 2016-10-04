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
        safeDelete(_program);

        if(_onDelete)
            _onDelete();
    }

    void renderPass::initialize()
    {
        if (_onInitialize)
            _onInitialize();
    }

    void renderPass::update()
    {
    }

    void renderPass::render()
    {
        if (_onCanRender && _onCanRender())
        {
            _onBeginRender(_program, _framebuffer, _resolution);
            _onRender(_vaos);
            _onEndRender(_program, _framebuffer, _resolution);
        }
    }

    void renderPass::resize(const resolution& resolution)
    {
        _resolution = resolution;

        if (_onResize)
            _onResize(resolution);
    }
}

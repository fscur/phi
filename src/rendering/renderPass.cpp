#include <precompiled.h>
#include "renderPass.h"

namespace phi
{
    renderPass::renderPass(const resolution& resolution) :
        _program(nullptr),
        _framebuffer(nullptr),
        _resolution(resolution)
    {
    }

    renderPass::~renderPass()
    {
        safeDelete(_program);
    }

    void renderPass::initialize()
    {
        onInitialize();
    }

    void renderPass::update()
    {
        onUpdate();
    }

    void renderPass::render()
    {
        onBeginRender();
        onRender();
        onEndRender();
    }

    void renderPass::resize(const resolution& resolution)
    {
        _resolution = resolution;
        onResize(resolution);
    }

    void renderPass::onRender()
    {
        for (auto vao : _vaos)
            vao->render();
    }
}

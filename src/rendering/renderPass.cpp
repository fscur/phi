#include <precompiled.h>
#include "renderPass.h"

namespace phi
{
    renderPass::renderPass(program* program) :
        _program(program)
    {
    }

    renderPass::~renderPass()
    {
    }

    void renderPass::update()
    {
        _onUpdate(_program);
    }

    void renderPass::render()
    {
        _onRender(_program);
    }
}

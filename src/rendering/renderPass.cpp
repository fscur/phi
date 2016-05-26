#include <precompiled.h>
#include "renderPass.h"

namespace phi
{
    renderPass::renderPass(shader* shader) :
        _shader(shader)
    {
    }

    renderPass::~renderPass()
    {
    }

    void renderPass::update()
    {
        _onUpdate(_shader);
    }

    void renderPass::render()
    {
        _onRender(_shader);
    }
}

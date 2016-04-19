#include <precompiled.h>
#include "controlRenderer.h"

namespace phi
{
    controlRenderer::controlRenderer(string name) :
        component(component::CONTROL_RENDERER, name),
        _geometry(nullptr),
        _backgroundTexture(nullptr),
        _backgroundColor(color::transparent)
    {
    }

    controlRenderer::~controlRenderer()
    {
    }
}
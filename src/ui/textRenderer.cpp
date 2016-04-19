#include <precompiled.h>
#include "textRenderer.h"

namespace phi
{
    textRenderer::textRenderer(string name) :
        component(component::TEXT_RENDERER, name),
        _text(L""),
        _font(nullptr),
        _backgroundColor(color::transparent),
        _foregroundColor(color::black)
    {
    }

    textRenderer::~textRenderer()
    {
    }
}
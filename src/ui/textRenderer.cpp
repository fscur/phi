#include <precompiled.h>
#include "textRenderer.h"

namespace phi
{
    textRenderer::textRenderer(string name) :
        component(component::TEXT_RENDERER, name),
        _font(nullptr),
        _controlRenderer(nullptr),
        _text(L""),
        _color(color::black)
    {
    }

    textRenderer::~textRenderer()
    {
    }

    void textRenderer::updateControlRenderer()
    {
        vec2 t = _font->measureText(_text);
        _controlRenderer->setSize(vec3(t, 0.0f));
    }

    inline void textRenderer::setText(wstring value) 
    { 
        _text = value; 
        updateControlRenderer();
    }

    inline void textRenderer::setFont(font * value)
    { 
        _font = value; 
        updateControlRenderer();
    }
}
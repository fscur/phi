#include <precompiled.h>
#include "text.h"

namespace phi
{
    text::text(string name) :
        component(component::TEXT_RENDERER, name),
        _font(nullptr),
        _controlRenderer(nullptr),
        _text(L""),
        _color(color::black)
    {
    }

    text::~text()
    {
    }

    void text::updateControlRenderer()
    {
        vec2 t = _font->measureText(_text);
        _controlRenderer->setSize(vec3(t, 0.0f));
    }

    inline void text::setText(wstring value) 
    { 
        _text = value; 
        updateControlRenderer();
    }

    inline void text::setFont(font * value)
    { 
        _font = value; 
        updateControlRenderer();
    }
}
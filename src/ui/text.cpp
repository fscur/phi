#include <precompiled.h>
#include "text.h"

namespace phi
{
    text::text(string name) :
        component(component::TEXT, name),
        _font(nullptr),
        _control(nullptr),
        _text(L""),
        _color(color::black)
    {
    }

    text::~text()
    {
    }

    void text::updateControl()
    {
        vec2 textSize = _font->measureText(_text);
        _control->setSize(vec3(textSize, 0.0f));
    }

    inline void text::setText(wstring value) 
    { 
        _text = value; 
        updateControl();
    }

    inline void text::setFont(font * value)
    { 
        _font = value; 
        updateControl();
    }
}
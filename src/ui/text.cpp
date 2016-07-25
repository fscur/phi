#include <precompiled.h>
#include "text.h"

namespace phi
{
    text::text() :
        component(component::TEXT),
        _font(nullptr),
        _control(nullptr),
        _text(L""),
        _color(color::black)
    {
    }

    text::text(const text& text) :
        component(componentType::TEXT),
        _font(text._font),
        _control(text._control),
        _text(text._text),
        _color(text._color)
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

    component* text::clone() const 
    {
        auto t = static_cast<const text*>(this);
        return new text(*t);
    }
}
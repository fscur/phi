#include <precompiled.h>
#include "text.h"

namespace phi
{
    text::text() :
        component(component::TEXT),
        textChangedEvent(eventHandler<text*>()),
        _font(nullptr),
        _control(nullptr),
        _text(L""),
        _color(color::black),
        _isBillboard(false)
    {
    }

    text::text(const text& original) :
        component(componentType::TEXT),
        textChangedEvent(eventHandler<text*>()),
        _font(original._font),
        _control(original._control),
        _text(original._text),
        _color(original._color),
        _isBillboard(original._isBillboard)
    {
    }

    text::~text()
    {
    }

    inline void text::setText(wstring value) 
    {
        _text = value; 
        updateControl();
        textChangedEvent.raise(this);
    }

    inline void text::setFont(font * value)
    { 
        _font = value; 
        updateControl();
    }

    void text::updateControl()
    {
        vec2 textSize = _font->measureText(_text);
        _control->setSize(vec3(textSize, 1.0f));
    }

    component* text::clone() const 
    {
        auto t = static_cast<const text*>(this);
        return new text(*t);
    }
}
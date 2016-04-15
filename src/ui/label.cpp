#include <precompiled.h>
#include "label.h"

namespace phi
{
    label::label() :
        control(controlType::label),
        _font(uiRepository::fontConsolas14),
        _text(L"[label]")
    {
    }

    label::~label()
    {
    }

    void label::updateTextLocation()
    {
        /*sizeui textSize = _textRenderer->measureSize(_text, _font);
        if (textSize.w > _size.w)
            _textX = _x;
        else
            _textX = (int)(_x + _size.w * 0.5f - textSize.w * 0.5f);

        _textY = (int)(_y + _size.h * 0.5f - textSize.h * 0.5f);*/
    }

    inline void label::setPosition(vec3 value)
    {
        _position = value;
        raisePropertyChanged();
    }

    inline void label::setSize(sizef value)
    {
        _size = value;
        raisePropertyChanged();
    }

    inline void label::setFont(font * value) 
    {
        _font = value;
        raisePropertyChanged();
    }

    inline void label::setText(wstring value)
    {
        _text = value;
        raisePropertyChanged();
    }
}
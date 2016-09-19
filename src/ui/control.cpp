#include <precompiled.h>
#include "control.h"

namespace phi
{
    control::control() :
        component(component::CONTROL),
        _image(image::defaultAlbedoImage),
        _onSizeChanged(new eventHandler<vec3>()),
        _color(color::transparent),
        _size(vec3(0.0f))
    {
    }

    control::control(const control& original) :
        component(component::CONTROL),
        _image(original._image),
        _onSizeChanged(new eventHandler<vec3>()),
        _color(original._color),
        _size(original._size)
    {
    }

    control::~control()
    {
        safeDelete(_onSizeChanged);
    }

    void control::setSize(vec3 value)
    {
        _size = value;
        _onSizeChanged->raise(value);
    }

    void control::setColor(color value)
    {
        _color = value;
        colorChanged.raise(this);
    }

    component* control::clone() const
    {
        auto c = static_cast<const control*>(this);
        return new control(*c);
    }
}
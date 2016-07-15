#include <precompiled.h>
#include "control.h"

namespace phi
{
    control::control(string name) :
        component(component::CONTROL, name),
        _image(image::defaultAlbedoImage),
        _onSizeChanged(new eventHandler<vec3>()),
        _color(color::transparent),
        _size(vec3(0.0f)),
        _isGlassy(false)
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
}
#include <precompiled.h>
#include "control.h"

namespace phi
{
    control::control(string name) :
        component(component::CONTROL_RENDERER, name),
        _geometry(nullptr),
        _texture(nullptr),
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
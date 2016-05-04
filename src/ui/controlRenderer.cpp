#include <precompiled.h>
#include "controlRenderer.h"

namespace phi
{
    controlRenderer::controlRenderer(string name) :
        component(component::CONTROL_RENDERER, name),
        _geometry(nullptr),
        _texture(nullptr),
        _onSizeChanged(new eventHandler<vec3>()),
        _color(color::transparent),
        _size(vec3(0.0f)),
		_isGlassy(false)
    {
    }

    controlRenderer::~controlRenderer()
    {
        safeDelete(_onSizeChanged);
    }

    void controlRenderer::setSize(vec3 value)
    {
        _size = value;
        _onSizeChanged->raise(value);
    }
}
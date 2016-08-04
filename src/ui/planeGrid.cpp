#include <precompiled.h>
#include "planeGrid.h"

namespace phi
{
    planeGrid::planeGrid() :
        component(component::PLANE_GRID),
        _colorChanged(eventHandler<planeGrid*>()),
        _lineThicknessChanged(eventHandler<planeGrid*>()),
        _opacityChanged(eventHandler<planeGrid*>()),
        _visibleChanged(eventHandler<planeGrid*>())
    {
    }

    planeGrid::planeGrid(const planeGrid& original) :
        component(componentType::PLANE_GRID),
        _color(original._color),
        _lineThickness(original._lineThickness),
        _opacity(original._opacity),
        _visible(true)
    {
    }

    planeGrid::~planeGrid()
    {
    }

    void planeGrid::setColor(const color& value)
    {
        if (_color == value)
            return;

        _color = value;
        _colorChanged.raise(this);
    }

    void planeGrid::setLineThickness(const float value)
    {
        if (_lineThickness == value)
            return;

        _lineThickness = value;
        _lineThicknessChanged.raise(this);
    }

    void planeGrid::setOpacity(const float value)
    {
        if (_opacity == value)
            return;

        _opacity = value;
        _opacityChanged.raise(this);
    }

    void planeGrid::show()
    {
        if (_visible)
            return;

        _visible = true;
        _visibleChanged.raise(this);
    }

    void planeGrid::hide()
    {
        if (!_visible)
            return;

        _visible = false;
        _visibleChanged.raise(this);
    }

    component* planeGrid::clone() const
    {
        auto p = static_cast<const planeGrid*>(this);
        return new planeGrid(*p);
    }
}
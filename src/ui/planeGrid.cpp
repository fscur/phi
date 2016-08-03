#include <precompiled.h>
#include "planeGrid.h"

namespace phi
{
    planeGrid::planeGrid() :
        component(component::PLANE_GRID),
        _visibleChanged(new eventHandler<planeGrid*>())
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

    void planeGrid::show()
    {
        if (_visible)
            return;

        _visible = true;
        _visibleChanged->raise(this);
    }

    void planeGrid::hide()
    {
        if (!_visible)
            return;

        _visible = false;
        _visibleChanged->raise(this);
    }

    component* planeGrid::clone() const
    {
        auto p = static_cast<const planeGrid*>(this);
        return new planeGrid(*p);
    }
}
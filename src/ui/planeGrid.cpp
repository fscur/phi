#include <precompiled.h>
#include "planeGrid.h"

namespace phi
{
    planeGrid::planeGrid() :
        component(component::PLANE_GRID)
    {
    }

    planeGrid::planeGrid(const planeGrid& original) :
        component(componentType::PLANE_GRID),
        _color(original._color),
        _lineThickness(original._lineThickness)
    {
    }

    planeGrid::~planeGrid()
    {
    }

    component* planeGrid::clone() const
    {
        auto p = static_cast<const planeGrid*>(this);
        return new planeGrid(*p);
    }
}
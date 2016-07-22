#include <precompiled.h>
#include "planeGrid.h"

namespace phi
{
    planeGrid::planeGrid(const string& name) :
        component(component::PLANE_GRID, name)
    {
    }

    planeGrid::planeGrid(const planeGrid& planeGrid) :
        component(componentType::PLANE_GRID, planeGrid._name),
        _image(planeGrid._image)
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
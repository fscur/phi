#include <precompiled.h>
#include "planeGrid.h"

namespace phi
{
    planeGrid::planeGrid() :
        component(component::PLANE_GRID)
    {
    }

    planeGrid::planeGrid(const planeGrid& planeGrid) :
        component(componentType::PLANE_GRID),
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
#include <precompiled.h>
#include "skyBox.h"

namespace phi
{
    skyBox::skyBox(color color) :
        component(component::SKY_BOX),
        _color(color)
    {
    }

    skyBox::skyBox(const skyBox& original) :
        component(componentType::SKY_BOX),
        _color(original._color)
    {
    }

    skyBox::~skyBox()
    {
    }

    component* skyBox::clone() const
    {
        auto p = static_cast<const skyBox*>(this);
        return new skyBox(*p);
    }
}
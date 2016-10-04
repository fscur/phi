#include <precompiled.h>
#include "skyBox.h"

namespace phi
{
    skyBox::skyBox(const skyBoxImages& images) :
        component(component::SKY_BOX),
        _images(images)
    {
    }

    skyBox::skyBox(const skyBox& original) :
        component(componentType::SKY_BOX),
        _images(original._images)
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
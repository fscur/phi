#pragma once
#include <phi.h>
#include "coreApi.h"
#include <core\component.h>
#include <core\color.h>

namespace phi
{
    class skyBox :
        public component
    {
    public:
        static componentType getComponentType()
        {
            return componentType::SKY_BOX;
        }

    public:
        CORE_API skyBox(color color);
        CORE_API skyBox(const skyBox& original);
        CORE_API ~skyBox();

        CORE_API component* clone() const override;
        color getColor() const { return _color; }
    private:
        color _color;
    };
}
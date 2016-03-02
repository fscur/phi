#pragma once
#include <precompiled.h>
#include <core\component.h>

namespace phi
{
    class model :
        public component
    {
    public:
        model(string name) :
            component(componentType::MODEL, name)
        {
        }

        model(const model& clone) :
            component(clone)
        {
        }
    };
}
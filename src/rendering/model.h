#pragma once

#include <core\component.h>

#include "rendering.h"

#include <string>

namespace phi
{
    class model :
        public component
    {
    public:
        model(std::string name) :
            component(componentType::MODEL, name)
        {
        }

        model(const model& clone) :
            component(clone)
        {
        }
    };
}
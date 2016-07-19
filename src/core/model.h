#pragma once
#include <phi.h>
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

        model(const model& original) :
            component(original)
        {
        }

        component* clone() const override
        {
            auto m = static_cast<const model*>(this);
            return new model(*m);
        }
    };
}
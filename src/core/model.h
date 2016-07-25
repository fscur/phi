#pragma once
#include <phi.h>
#include <core\component.h>

namespace phi
{
    class model :
        public component
    {
    public:
        model() :
            component(componentType::MODEL)
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
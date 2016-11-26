#pragma once
#include <phi.h>
#include <core/component.h>

namespace phi
{
    class modelNode :
        public component
    {
    public:
        modelNode() :
            component(componentType::MODEL_NODE)
        {
        }

        modelNode(const modelNode& original) :
            component(original)
        {
        }

        component* clone() const override
        {
            auto m = static_cast<const modelNode*>(this);
            return new modelNode(*m);
        }

        static componentType getComponentType() { return componentType::MODEL_NODE; }
    };
}
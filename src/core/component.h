#pragma once
#include <precompiled.h>
#include "transform.h"

namespace phi
{
    class component
    {
    public:
        enum componentType
        {
            MODEL,
            MESH,
            CAMERA,
            DIRECTIONAL_LIGHT,
            POINT_LIGHT,
            SPOT_LIGHT
        };

    protected:
        componentType _type;
        std::string _name;

    protected:
        component(componentType type, std::string name) :
            _type(type), _name(name) {}

    public:
        virtual component* clone() { return new component(*this); }

        int getType() { return _type; }
        std::string getName() { return _name; }
    };
}
#pragma once
#include <precompiled.h>
#include "transform.h"

#include <string>

namespace phi
{
    class node;

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
        node* _node;

    protected:
        component(componentType type, std::string name) :
            _type(type), _name(name), _node(nullptr) { }

    public:
        virtual component* clone() const { return new component(*this); }

        int getType() const { return _type; }
        std::string getName() const { return _name; }

        void setNode(node* value) { _node = value; }
    };
}
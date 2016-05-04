#pragma once
#include <phi.h>
#include "transform.h"

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
        string _name;
        node* _node;

    protected:
        component(componentType type, string name) :
            _type(type),
            _name(name),
            _node(nullptr)
        {
        }

        void virtual onNodeChanged(node* previousValue) {}

    public:
        virtual ~component() {}

        virtual component* clone() const { return new component(*this); }

        int getType() const { return _type; }

        string getName() const { return _name; }

        void setNode(node* value)
        {
            auto previousValue = _node;
            _node = value;
            onNodeChanged(previousValue);
        }

        node* getNode() { return _node; }
    };
}
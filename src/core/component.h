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
            SPOT_LIGHT,
            CONTROL,
            TEXT,
            CLICK
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

    public:
        virtual ~component() {};

        virtual component* clone() const { return new component(*this); }

        int getType() const { return _type; }

        string getName() const { return _name; }

        void virtual onNodeChanged(node* previousValue) { }

        node* getNode() const { return _node; }

        void setNode(node* value)
        {
            auto previousValue = _node;
            _node = value;
            onNodeChanged(previousValue);
        }

        virtual bool operator==(const component& other)
        {
            return _name == other._name &&
                _type == other._type;
        }
        
        virtual bool operator!=(const component& other)
        {
            return !(*this == other);
        }
    };
}
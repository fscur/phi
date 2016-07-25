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
            CLICK,
            BOX_COLLIDER,
            PLANE_GRID
        };

    protected:
        componentType _type;
        node* _node;

    protected:
        component(componentType type) :
            _type(type),
            _node(nullptr)
        {
        }

    public:
        virtual ~component() {};

        virtual component* clone() const = 0;

        int getType() const { return _type; }

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
            return _type == other._type; //TODO::WTF???
        }
        
        virtual bool operator!=(const component& other)
        {
            return !(*this == other);
        }
    };
}
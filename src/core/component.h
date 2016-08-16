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
            MODEL = 0,
            MESH = 1,
            CAMERA = 2,
            DIRECTIONAL_LIGHT = 3,
            POINT_LIGHT = 4,
            SPOT_LIGHT = 5,
            CONTROL = 6,
            TEXT = 7,
            CLICK = 8,
            BOX_COLLIDER = 9,
            PLANE_GRID = 10,
            ANIMATOR = 11,
            GHOST_MESH = 12
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
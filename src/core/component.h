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
            CONTROL_RENDERER,
            TEXT_RENDERER
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
        virtual ~component() 
        {
        }

        virtual component* clone() const { return new component(*this); }

        int getType() const { return _type; }

        string getName() const { return _name; }

        void virtual onNodeChanged(node* previousValue) { }

        node* getNode() { return _node; }

        CORE_API void setNode(node* value);

        CORE_API virtual bool operator==(const component& other);
        CORE_API virtual bool operator!=(const component& other);
    };
}
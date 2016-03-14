#pragma once
#include <phi.h>
#include "component.h"
#include "transform.h"

#include <vector>

namespace phi
{
    class node
    {
    private:
        transform _transform;
        vector<component*> _components;
        node* _parent;
        vector<node*> _children;

    public:
        CORE_API node();
        CORE_API node(const node& original);

        CORE_API node* clone() const;

        CORE_API void addComponent(component* const component);
        CORE_API void addChild(node* const child);

        transform& getTransform() { return _transform; }
        node* getParent() const { return _parent; }
        vector<node*>& getChildren() { return _children; }
        void setParent(node* const value) { _parent = value; }

        template<typename T>
        T* getComponent() const
        {
            const component::componentType type = T::getComponentType();
            for (unsigned int i = 0; i < _components.size(); i++)
            {
                if (_components[i]->getType() == type)
                    return static_cast<T*>(_components[i]);
            }

            return nullptr;
        }
    };
}
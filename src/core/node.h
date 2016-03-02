#pragma once

#include "transform.h"

#include <vector>

#include <core\component.h>

namespace phi
{
    class node
    {
    private:
        transform _transform;
        std::vector<component*> _components;
        node* _parent;
        std::vector<node*> _children;

    public:
        node() :
            _parent(nullptr), _transform(transform()) {}

        node(const node& original) :
            _transform(*(original._transform.clone())),
            _parent(nullptr)
        {
            auto clonedChildren = std::vector<node*>();
            for (auto& child : original._children)
            {
                auto clonedChild = child->clone();
                clonedChild->_parent = this;
                clonedChild->getTransform().setParent(&_transform);
                clonedChildren.push_back(clonedChild);
            }
            _children = std::move(clonedChildren);

            auto clonedComponents = std::vector<component*>();
            for (auto& component : original._components)
            {
                auto clonedComponent = component->clone();
                clonedComponents.push_back(clonedComponent);
            }
            _components = std::move(clonedComponents);
        }

        node* clone() const { return new node(*this); }
        transform& getTransform() { return _transform; }
        node* getParent() const { return _parent; }
        std::vector<node*>& getChildren() { return _children; }

        void setParent(node* const value) { _parent = value; }

        void addComponent(component* const component)
        {
            _components.push_back(component);
            component->setNode(this);
        }

        void addChild(node* const child)
        {
            _children.push_back(child);
            child->setParent(this);
        }

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
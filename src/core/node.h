#pragma once

#include "component.h"
#include "transform.h"

#include <vector>

namespace phi
{
    class node
    {
    private:
        transform* _transform;
        std::vector<component*> _components;
        node* _parent;
        std::vector<node*> _children;

    public:
        node() :
            _parent(nullptr), _transform(new transform()) {}

        node(const node& original) :
            _transform(original._transform->clone()),
            _parent(nullptr)
        {
            auto clonedChildren = std::vector<node*>();
            for (auto& child : original._children)
            {
                auto clonedChild = child->clone();
                clonedChild->_parent = this;
                clonedChild->getTransform()->setParent(_transform);
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

        node* clone() { return new node(*this); }
        transform* getTransform() { return _transform; }
        node* getParent() { return _parent; }
        std::vector<node*> getChildren() { return _children; }

        void setParent(node* value) { _parent = value; }

        void addComponent(component* component) { _components.push_back(component); }
        void addChild(node* child)
        {
            _children.push_back(child);
            child->setParent(this);
        }

        component* getComponent(component::componentType type)
        {
            for (unsigned int i = 0; i < _components.size(); i++)
            {
                if (_components[i]->getType() == type)
                    return _components[i];
            }

            return nullptr;
        }
    };
}
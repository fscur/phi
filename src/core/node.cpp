#include <precompiled.h>
#include "node.h"

namespace phi
{
    node::node() :
        _parent(nullptr), 
        _transform(transform()) 
    {
    }

    node::~node()
    {
        for (auto child : _children)
            safeDelete(child);

        for (auto component : _components)
            safeDelete(component);
    }

    node::node(const node& original) :
        _transform(original._transform),
        _parent(nullptr)
    {
        auto clonedChildren = vector<node*>();
        for (auto& child : original._children)
        {
            auto clonedChild = child->clone();
            clonedChild->_parent = this;
            clonedChild->getTransform().setParent(&_transform);
            clonedChildren.push_back(clonedChild);
        }
        _children = std::move(clonedChildren);

        auto clonedComponents = vector<component*>();
        for (auto& component : original._components)
        {
            auto clonedComponent = component->clone();
            clonedComponents.push_back(clonedComponent);
        }
        _components = std::move(clonedComponents);
    }

    inline node* node::clone() const 
    { 
        return new node(*this); 
    }

    inline void node::addComponent(component* const component)
    {
        _components.push_back(component);
        component->setNode(this);
    }

    inline void node::addChild(node* const child)
    {
        _children.push_back(child);
        child->setParent(this);
    }
}
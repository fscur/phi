#include <precompiled.h>
#include "node.h"

namespace phi
{
    node::node() :
        _parent(nullptr),
        _transform(new transform()),
        _components(vector<component*>()),
        _children(vector<node*>())
    {
    }

    node::~node()
    {
        for (auto child : _children)
            safeDelete(child);

        for (auto component : _components)
            safeDelete(component);

        safeDelete(_transform);
    }

    node::node(const node& original) :
        _parent(nullptr),
        _transform(original._transform->clone())
    {
        for (auto& child : original._children)
        {
            auto clonedChild = child->clone();
            clonedChild->_parent = this;
            clonedChild->getTransform()->setParent(_transform);
            _children.push_back(clonedChild);
        }

        for (auto& component : original._components)
        {
            auto clonedComponent = component->clone();
            _components.push_back(clonedComponent);
        }
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

    inline void node::setPosition(vec3 value)
    {
        _transform->setLocalPosition(value);
    }

    inline void node::setSize(vec3 value)
    {
        _transform->setLocalSize(value);
    }
}
#include <precompiled.h>
#include "node.h"

namespace phi
{
    node::node() :
        _parent(nullptr),
        _transform(new transform()),
        _components(vector<component*>()),
        _children(vector<node*>()),
        _transformChanged(new eventHandler<node*>())
    {
        _transform->getChangedEvent()->assign(std::bind(&node::transformChanged, this, std::placeholders::_1));
    }

    node::~node()
    {
        for (auto child : _children)
            safeDelete(child);

        for (auto component : _components)
            safeDelete(component);

        safeDelete(_transform);
        safeDelete(_transformChanged);
    }

    node::node(const node& original) :
        _parent(nullptr),
        _transform(original._transform->clone()),
        _transformChanged(new eventHandler<node*>())
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

        _transform->getChangedEvent()->assign(std::bind(&node::transformChanged, this, std::placeholders::_1));
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

    void node::traverse(std::function<void(node*)> func)
    {
        func(this);

        for (auto child : _children)
            child->traverse(func);
    }

    inline void node::transformChanged(transform* sender)
    {
        _transformChanged->raise(this);
    }
}
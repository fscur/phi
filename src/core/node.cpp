#include <precompiled.h>
#include "node.h"

namespace phi
{
    node::node(string name) :
        _parent(nullptr),
        _transform(new transform()),
        _components(new vector<component*>()),
        _children(new vector<node*>()),
        _transformChanged(new eventHandler<node*>()),
        _name(name)
    {
        _transform->getChangedEvent()->assign(std::bind(&node::transformChanged, this, std::placeholders::_1));
    }

    node::~node()
    {
        for (auto child : *_children)
            safeDelete(child);

        for (auto component : *_components)
            safeDelete(component);

        safeDelete(_children);
        safeDelete(_components);
        safeDelete(_transform);
        safeDelete(_transformChanged);
    }

    node::node(const node& original) :
        _parent(nullptr),
        _transform(original._transform->clone()),
        _components(new vector<component*>()),
        _children(new vector<node*>()),
        _transformChanged(new eventHandler<node*>()),
        _name(original._name)
    {
        for (auto child : *(original._children))
        {
            auto clonedChild = child->clone();
            clonedChild->_parent = this;
            clonedChild->getTransform()->setParent(_transform);

            _children->push_back(clonedChild);
        }

        for (auto component : *(original._components))
        {
            auto clonedComponent = component->clone();
            clonedComponent->setNode(this);
            _components->push_back(clonedComponent);
        }

        _transform->getChangedEvent()->assign(std::bind(&node::transformChanged, this, std::placeholders::_1));
    }

    inline node* node::clone() const
    {
        return new node(*this);
    }

    inline void node::addComponent(component* const component)
    {
        _components->push_back(component);
        component->setNode(this);
    }

    inline void node::addChild(node* const child)
    {
        _children->push_back(child);
        child->setParent(this);
    }

    void node::removeChild(node* child)
    {
        auto it = std::find(_children->begin(), _children->end(), child);
        if (it != _children->end())
        {
            _children->erase(it);
        }
    }

    void node::traverse(std::function<void(node*)> func)
    {
        func(this);

        for (auto child : *_children)
            child->traverse(func);
    }

    inline void node::transformChanged(transform* sender)
    {
        _transformChanged->raise(this);
    }

    inline void node::setPosition(vec3 value)
    {
        _transform->setLocalPosition(value);
    }

    inline void node::setSize(vec3 value)
    {
        _transform->setLocalSize(value);
    }

    inline void node::removeEmptyNodes()
    {
        //TODO: safeDelete stuff

        vector<node*> removeEmptyNodes;

        for (auto child : *_children)
        {
            auto grandsonsCount = child->_children->size();
            auto grandsonsComponentsCount = child->_components->size();

            if (grandsonsCount == 0 && grandsonsComponentsCount == 0)
                removeEmptyNodes.push_back(child);

            child->removeEmptyNodes();
        }

        for (auto nodeToRemove : removeEmptyNodes)
        {
            auto children = _children;
            auto it = std::find(children->begin(), children->end(), nodeToRemove);
            children->erase(it);
        }
    }

    inline void node::removeUselessNodes()
    {
        auto childrenCount = _children->size();

        for (size_t i = 0; i < childrenCount; ++i)
        {
            auto child = _children->at(i);

            bool doesNotHaveComponents = child->_components->size() == 0;
            bool hasOnlyOneChild = child->_children->size() == 1;

            if (doesNotHaveComponents && hasOnlyOneChild)
            {
                auto next = child;

                while (next->_children->size() == 1 && _components->size() == 0)
                {
                    //TODO: safeDelete excluded nodes
                    next = next->_children->at(0);
                }

                next->_parent = this;
                (*_children)[i] = next;
                child = (*_children)[i];
            }

            child->removeUselessNodes();
        }
    }

    inline void node::optimize()
    {
        removeEmptyNodes();
        removeUselessNodes();

        bool doesNotHaveComponents = _components->size() == 0;
        bool hasOnlyOneChild = _children->size() == 1;

        if (doesNotHaveComponents && hasOnlyOneChild)
        {
            auto grandsons = _children->at(0)->_children;
            _children = grandsons;

            auto childrenCount = _children->size();
            for (size_t i = 0; i < childrenCount; ++i)
            {
                auto child = _children->at(i);
                child->_parent = this;
            }
        }
    }
}
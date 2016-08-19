#include <precompiled.h>
#include "node.h"

namespace phi
{
    node::node(string name) :
        _parent(nullptr),
        _transform(new transform()),
        _components(new vector<component*>()),
        _children(new vector<node*>()),
        _name(name),
        _isSelected(false)
    {
        _transform->getChangedEvent()->assign(std::bind(&node::raiseTransformChanged, this, std::placeholders::_1));
    }

    node::node(const node& original) :
        _parent(nullptr),
        _transform(original._transform->clone()),
        _components(new vector<component*>()),
        _children(new vector<node*>()),
        _name(original._name),
        _isSelected(original._isSelected)
    {
        for (auto child : *original._children)
        {
            auto clonedChild = child->clone();
            clonedChild->_parent = this;
            clonedChild->getTransform()->setParent(_transform);

            _children->push_back(clonedChild);
        }

        for (auto component : *original._components)
        {
            auto clonedComponent = component->clone();
            clonedComponent->setNode(this);
            _components->push_back(clonedComponent);
        }

        _transform->getChangedEvent()->assign(std::bind(&node::raiseTransformChanged, this, std::placeholders::_1));
    }

    node::~node()
    {
        for (auto child : *_children)
            safeDelete(child);

        safeDelete(_children);

        for (auto component : *_components)
            safeDelete(component);

        safeDelete(_components);
        safeDelete(_transform);
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

        childAdded.raise(child);
    }

    void node::removeChild(node* child)
    {
        auto iterator = std::find(_children->begin(), _children->end(), child);
        if (iterator != _children->end())
        {
            _children->erase(iterator);
            childRemoved.raise(child);
        }
    }

    void node::clearChildren()
    {
        for (auto child : *_children)
            child->setParent(nullptr);

        _children->clear();
    }

    void node::traverse(std::function<void(node*)> func)
    {
        func(this);

        for (auto child : *_children)
            child->traverse(func);
    }

    inline void node::raiseTransformChanged(transform* sender)
    {
        transformChanged.raise(this);
    }

    inline void node::setIsSelected(bool isSelected) 
    {
        _isSelected = isSelected;
        selectionChanged.raise(this);
    }

    inline void node::setIsTranslating(bool value)
    {
        _isTranslating = value;
    }

    inline void node::setParent(node * const value)
    {
        _parent = value;

        if (_parent)
            _transform->setParent(_parent->getTransform());
        else
            _transform->setParent(nullptr);
    }

    inline void node::setPosition(vec3 value)
    {
        _transform->setLocalPosition(value);
    }

    inline void node::setSize(vec3 value)
    {
        _transform->setLocalSize(value);
    }

    bool node::operator==(const node& other)
    {
        auto otherComponents = other.getComponents();
        auto componentsCount = _components->size();
        auto otherComponentsCount = otherComponents->size();

        if (componentsCount != otherComponentsCount)
            return false;

        for (auto i = 0u; i < componentsCount; i++)
        {
            if (*(_components->at(i)) != *(otherComponents->at(i)))
                return false;
        }

        auto otherChildren = other.getChildren();
        auto childrenCount = _children->size();
        auto otherChildrenCount = otherChildren->size();

        if (childrenCount != otherChildrenCount)
            return false;

        for (auto i = 0u; i < childrenCount; i++)
        {
            if (*(_children->at(i)) != *(otherChildren->at(i)))
                return false;
        }

        if (*_transform != *other._transform)
            return false;

        if (_parent && other._parent)
        {
            auto parent = _parent;
            auto otherParent = other._parent;
            while (parent && otherParent)
            {
                if (parent->_name != otherParent->_name)
                    return false;

                parent = parent->_parent;
                otherParent = otherParent->_parent;
            }

            if (parent != otherParent)
                return false;
        }
        else if (_parent != other.getParent())
        {
            return false;
        }

        if (_name != other._name)
            return false;

        return true;
    }

    bool node::operator!=(const node& other)
    {
        return !(*this == other);
    }
}
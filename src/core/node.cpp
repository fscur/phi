#include <precompiled.h>
#include "node.h"

namespace phi
{
    node::node(string name) :
        _parent(nullptr),
        _transform(new transform()),
        _components(vector<component*>()),
        _children(vector<node*>()),
        _name(name),
        _localObb(nullptr),
        _obb(nullptr),
        _isSelected(false),
        _isTranslating(false)
    {
        _transform->getChangedEvent()->assign(std::bind(&node::raiseTransformChanged, this, std::placeholders::_1));
    }

    node::node(const node& original) :
        _parent(nullptr),
        _transform(original._transform->clone()),
        _components(vector<component*>()),
        _children(vector<node*>()),
        _name(original._name),
        _localObb(nullptr),
        _obb(nullptr),
        _isSelected(original._isSelected),
        _isTranslating(original._isTranslating)
    {
        if (original._localObb)
            _localObb = new obb(*original._localObb);
        if (original._obb)
            _obb = new obb(*original._obb);

        for (auto child : original._children)
        {
            auto clonedChild = child->clone();
            clonedChild->_parent = this;
            clonedChild->getTransform()->setParent(_transform);

            _children.push_back(clonedChild);
        }

        for (auto component : original._components)
        {
            auto clonedComponent = component->clone();
            clonedComponent->setNode(this);
            _components.push_back(clonedComponent);
        }

        _transform->getChangedEvent()->assign(std::bind(&node::raiseTransformChanged, this, std::placeholders::_1));
    }

    node::~node()
    {
        for (auto child : _children)
            safeDelete(child);

        for (auto component : _components)
            safeDelete(component);

        safeDelete(_transform);
        safeDelete(_obb);
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

        auto transformChangedToken = child->transformChanged.assign(std::bind(&node::onChildTransformChanged, this, std::placeholders::_1));
        _childrenEventTokens[child].transformChangedToken = transformChangedToken;

        auto localObbChangedToken = child->localObbChanged.assign(std::bind(&node::onLocalObbChanged, this, std::placeholders::_1));
        _childrenEventTokens[child].localObbChangedToken = localObbChangedToken;

        childAdded.raise(child);
        updateObb();
    }

    void node::removeChild(node* child)
    {
        auto iterator = std::find(_children.begin(), _children.end(), child);
        if (iterator != _children.end())
        {
            _children.erase(iterator);
            child->setParent(nullptr);
            child->transformChanged.unassign(_childrenEventTokens[child].transformChangedToken);
            child->localObbChanged.unassign(_childrenEventTokens[child].localObbChangedToken);
            _childrenEventTokens.erase(child);
            childRemoved.raise(child);
        }

        updateObb();
    }

    void node::clearChildren()
    {
        while (_children.size() > 0)
            removeChild(_children[0]);
    }

    void node::traverse(std::function<void(node*)> func)
    {
        func(this);

        for (auto& child : _children)
            child->traverse(func);
    }

    void node::updateObb()
    {
        if (_children.size() == 0)
        {
            if (_obb)
                safeDelete(_obb);

            if (_localObb)
                _obb = new obb(*_localObb);

            return;
        }

        vec3 min;
        vec3 max;
        if (_localObb)
            _localObb->getLimits(min, max);
        else
        {
            auto minFloat = std::numeric_limits<float>().min();
            auto maxFloat = std::numeric_limits<float>().max();
            min = vec3(maxFloat, maxFloat, maxFloat);
            max = vec3(minFloat, minFloat, minFloat);
        }

        for (auto& child : _children)
        {
            auto childObb = child->getObb();
            if (!childObb)
                continue;

            vec3 childMin;
            vec3 childMax;
            childObb->getLimits(childMin, childMax);

            if (childMin.x < min.x)
                min.x = childMin.x;
            if (childMin.y < min.y)
                min.y = childMin.y;
            if (childMin.z < min.z)
                min.z = childMin.z;

            if (childMax.x > max.x)
                max.x = childMax.x;
            if (childMax.y > max.y)
                max.y = childMax.y;
            if (childMax.z > max.z)
                max.z = childMax.z;
        }

        auto center = (min + max) * 0.5f;
        auto width = max.x - min.x;
        auto height = max.y - min.y;
        auto depth = max.z - min.z;

        obb obb;
        obb.center = center;
        obb.axes[0] = vec3(1.0f, 0.0f, 0.0f);
        obb.axes[1] = vec3(0.0f, 1.0f, 0.0f);
        obb.axes[2] = vec3(0.0f, 0.0f, 1.0f);
        obb.halfSizes = vec3(width, height, depth);
        obb = obb.transform(getTransform());

        if (!_obb)
            _obb = new phi::obb(obb);
        else
        {
            _obb->center = obb.center;
            _obb->axes[0] = obb.axes[0];
            _obb->axes[1] = obb.axes[1];
            _obb->axes[2] = obb.axes[2];
            _obb->halfSizes = obb.halfSizes;
        }
    }

    inline void node::raiseTransformChanged(transform* transform)
    {
        updateObb();
        transformChanged.raise(this);
    }

    void node::onChildTransformChanged(node* child)
    {
        updateObb();
    }

    void node::onLocalObbChanged(node* child)
    {
        updateObb();
    }

    inline void node::select() 
    {
        if (_isSelected)
            return;

        _isSelected = true;
        selectionChanged.raise(this);
    }

    inline void node::deselect()
    {
        if (!_isSelected)
            return;

        _isSelected = false;
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

    inline void node::setLocalObb(obb* value)
    {
        if (_localObb)
            safeDelete(_localObb);

        _localObb = value;
        updateObb();
        localObbChanged.raise(this);
    }

    bool node::operator==(const node& other)
    {
        auto otherComponents = other.getComponents();
        auto componentsCount = _components.size();
        auto otherComponentsCount = otherComponents->size();

        if (componentsCount != otherComponentsCount)
            return false;

        for (auto i = 0u; i < componentsCount; i++)
        {
            if (*_components[i] != *(otherComponents->at(i)))
                return false;
        }

        auto otherChildren = other.getChildren();
        auto childrenCount = _children.size();
        auto otherChildrenCount = otherChildren->size();

        if (childrenCount != otherChildrenCount)
            return false;

        for (auto i = 0u; i < childrenCount; i++)
        {
            if (*_children[i] != *(otherChildren->at(i)))
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
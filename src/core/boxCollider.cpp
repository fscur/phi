#include <precompiled.h>
#include <core\node.h>
#include "boxCollider.h"

namespace phi
{
    boxCollider::boxCollider(vec3 center, vec3 size) :
        component(component::componentType::BOX_COLLIDER),
        _center(center),
        _size(vec3(glm::max(size.x, DECIMAL_TRUNCATION), glm::max(size.y, DECIMAL_TRUNCATION), glm::max(size.z, DECIMAL_TRUNCATION))),
        _obb(obb(center, vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), size * 0.5f)),
        _transformChangedEventToken(eventToken()),
        _modelMatrix(mat4(1.0)),
        _isEnabled(true),
        _isEnabledChanged(eventHandler<boxCollider*>())
    {
    }

    boxCollider::boxCollider(const boxCollider& original) :
        component(component::componentType::BOX_COLLIDER),
        _center(original._center),
        _size(original._size),
        _obb(original._obb),
        _transformChangedEventToken(eventToken()),
        _modelMatrix(original._modelMatrix),
        _isEnabled(original._isEnabled),
        _isEnabledChanged(eventHandler<boxCollider*>())
    {
    }

    boxCollider::~boxCollider()
    {
        if (_node)
            _node->getTransform()->getChangedEvent()->unassign(_transformChangedEventToken);
    }

    inline transform* boxCollider::getTransform()
    {
        if (_node == nullptr)
            return nullptr;

        return _node->getTransform();
    }

    void boxCollider::updateModelMatrix()
    {
        auto translationMatrix = glm::translate(_center);
        auto scaleMatrix = glm::scale(_size);

        auto transform = getTransform();
        _modelMatrix = transform->getModelMatrix() * translationMatrix * scaleMatrix;

        auto localObb = obb(_center, vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), _size * 0.5f);
        _obb = localObb.transform(transform);
    }

    inline void boxCollider::nodeTransformChanged(transform* const sender)
    {
        updateModelMatrix();
    }

    void boxCollider::raiseIsEnabledChanged()
    {
        _isEnabledChanged.raise(this);
    }

    void boxCollider::enable()
    {
        _isEnabled = true;
        raiseIsEnabledChanged();
    }

    void boxCollider::disable()
    {
        _isEnabled = false;
        raiseIsEnabledChanged();
    }

    inline void boxCollider::onNodeChanged(node* previousValue)
    {
        if (previousValue)
            previousValue->getTransform()->getChangedEvent()->unassign(_transformChangedEventToken);

        if (_node)
        {
            _transformChangedEventToken = _node->getTransform()->getChangedEvent()->assign(std::bind(&boxCollider::nodeTransformChanged, this, std::placeholders::_1));
            nodeTransformChanged(_node->getTransform());
        }
        else
        {
            auto identity = transform();
            nodeTransformChanged(&identity);
        }
    }

    component* boxCollider::clone() const
    {
        auto b = static_cast<const boxCollider*>(this);
        return new boxCollider(*b);
    }
}
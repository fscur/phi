#include <precompiled.h>
#include <core\node.h>
#include "boxCollider.h"

namespace phi
{
    boxCollider::boxCollider(std::string name, vec3 position, vec3 halfSizes) :
        component(component::componentType::BOX_COLLIDER, name),
        _position(position),
        _halfSizes(vec3(glm::max(halfSizes.x, DECIMAL_TRUNCATION), glm::max(halfSizes.y, DECIMAL_TRUNCATION), glm::max(halfSizes.z, DECIMAL_TRUNCATION))),
        _obb(obb(position, vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), halfSizes)),
        _transformChangedEventToken(eventToken()),
        _localModelMatrix(mat4(1.0)),
        _worldModelMatrix(mat4(1.0))
    {
    }

    boxCollider::boxCollider(const boxCollider& original) :
        component(component::componentType::BOX_COLLIDER, original._name),
        _position(original._position),
        _halfSizes(original._halfSizes),
        _obb(original._obb),
        _transformChangedEventToken(eventToken()),
        _localModelMatrix(original._localModelMatrix),
        _worldModelMatrix(original._worldModelMatrix)
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

    inline void boxCollider::transformChanged(transform* const sender)
    {
        auto rotation = sender->getOrientation();
        auto center = rotation * _position;
        auto position = center + sender->getPosition();
        _obb = obb(position, sender->getRight(), sender->getUp(), sender->getDirection(), _halfSizes);

        auto translationMatrix = glm::translate(_obb.center);
        auto rotationMatrix = glm::mat4(
            vec4(_obb.axes[0], 0.0f),
            vec4(_obb.axes[1], 0.0f),
            vec4(_obb.axes[2], 0.0f),
            vec4(0.0f, 0.0f, 0.0f, 1.0f));

        auto scaleMatrix = glm::scale(_halfSizes * 2.0f);

        _localModelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
    }

    inline void boxCollider::onNodeChanged(node* previousValue)
    {
        if (previousValue)
            previousValue->getTransform()->getChangedEvent()->unassign(_transformChangedEventToken);

        if (_node)
        {
            _transformChangedEventToken = _node->getTransform()->getChangedEvent()->assign(std::bind(&boxCollider::transformChanged, this, std::placeholders::_1));
            transformChanged(_node->getTransform());
        }
        else
        {
            auto identity = transform();
            transformChanged(&identity);
        }
    }

    component* boxCollider::clone() const
    {
        auto b = static_cast<const boxCollider*>(this);
        return new boxCollider(*b);
    }
}
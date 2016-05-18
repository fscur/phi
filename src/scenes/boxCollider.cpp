#include <precompiled.h>
#include <core\node.h>
#include "boxCollider.h"

namespace phi
{
    boxCollider::boxCollider(std::string name, vec3 position, vec3 halfSizes) :
        component(component::componentType::BOX_COLLIDER, name),
        _position(position),
        _halfSizes(halfSizes),
        _obb(obb(position, vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), halfSizes)),
        _transformChangedEventToken(eventToken())
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
        auto rotation = sender->getLocalOrientation();
        auto center = rotation * _position;
        auto position = center + sender->getPosition();
        _obb = obb(position, sender->getRight(), sender->getUp(), sender->getDirection(), _halfSizes);
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
}
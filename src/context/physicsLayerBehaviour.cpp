#include <precompiled.h>

#include <core\boxCollider.h>

#include "physicsLayerBehaviour.h"

namespace phi
{
    void physicsLayerBehaviour::onNodeAdded(node* node)
    {
        auto collider = node->getComponent<boxCollider>();

        if (collider)
            _physicsWorld->addCollider(collider);
    }

    void physicsLayerBehaviour::onNodeRemoved(node* node)
    {
        auto collider = node->getComponent<boxCollider>();

        if (collider)
            _physicsWorld->removeCollider(collider);
    }
}
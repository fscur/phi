#include <precompiled.h>

#include <core\boxCollider.h>

#include "physicsLayerBehaviour.h"

namespace phi
{
    physicsLayerBehaviour::physicsLayerBehaviour() :
        _physicsWorld(new physicsWorld())
    {
    }

    physicsLayerBehaviour::~physicsLayerBehaviour()
    {
        safeDelete(_physicsWorld);
    }

    void physicsLayerBehaviour::onNodeAdded(node* node)
    {
        auto boxCollider = node->getComponent<phi::boxCollider>();

        if (boxCollider)
            _physicsWorld->addCollider(boxCollider);
    }

    void physicsLayerBehaviour::onNodeRemoved(node* node)
    {
        auto boxCollider = node->getComponent<phi::boxCollider>();

        if (boxCollider)
            _physicsWorld->removeCollider(boxCollider);
    }
}
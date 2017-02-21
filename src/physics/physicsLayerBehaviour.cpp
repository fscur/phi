#include <precompiled.h>

#include <core\boxCollider.h>
#include <core\mesh.h>
#include <animation\animator.h>

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

        if (!boxCollider)
        {
            auto mesh = node->getComponent<phi::mesh>();
            if (mesh)
            {
                auto aabb = mesh->getGeometry()->aabb;
                boxCollider = new phi::boxCollider(aabb->center, vec3(aabb->width, aabb->height, aabb->depth));
                node->addComponent(boxCollider);
                auto obb = new phi::obb(aabb->center, vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec3(aabb->halfWidth, aabb->halfHeight, aabb->halfDepth));
                node->setLocalObb(obb);
            }
        }

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
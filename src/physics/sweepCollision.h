#pragma once

#include <phi.h>

#include <core\boxCollider.h>

namespace phi
{
    struct sweepCollision
    {
        sweepCollision() :
            sourceCollider(nullptr),
            collider(nullptr),
            distance(0.0f),
            normal(vec3())
        {
        }

        sweepCollision(boxCollider* sourceCollider, boxCollider* collider, float distance, vec3 normal) :
            sourceCollider(sourceCollider),
            collider(collider),
            distance(distance),
            normal(normal)
        {
        }

        boxCollider* sourceCollider;
        boxCollider* collider;
        float distance;
        vec3 normal;

        bool operator ==(const sweepCollision& b)
        {
            return
                this->sourceCollider == b.sourceCollider &&
                this->collider == b.collider &&
                this->distance == b.distance &&
                this->normal == b.normal;
        }

        bool operator !=(const sweepCollision& b)
        {
            return !(*this == b);
        }
    };
}
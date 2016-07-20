#pragma once

#include <phi.h>

#include <core\boxCollider.h>

namespace phi
{
    struct sweepCollision
    {
        sweepCollision() :
            collider(nullptr),
            distance(0.0f),
            normal(vec3())
        {
        }

        sweepCollision(boxCollider* collider, float distance, vec3 normal) :
            collider(collider),
            distance(distance),
            normal(normal)
        {
        }

        boxCollider* collider;
        float distance;
        vec3 normal;

        bool operator ==(const sweepCollision& b)
        {
            return
                this->distance == b.distance &&
                this->normal == b.normal &&
                this->collider == b.collider;
        }

        bool operator !=(const sweepCollision& b)
        {
            return !(*this == b);
        }
    };
}
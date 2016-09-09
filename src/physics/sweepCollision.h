#pragma once

#include <phi.h>

#include <core\boxCollider.h>

namespace phi
{
    struct sweepCollision
    {
        sweepCollision() :
            collider(nullptr),
            collidee(nullptr),
            position(vec3()),
            distance(0.0f),
            normal(vec3()),
            isIntersecting(false)
        {
        }

        boxCollider* collider;
        boxCollider* collidee;
        vec3 position;
        float distance;
        vec3 normal;
        bool isIntersecting;

        bool operator ==(const sweepCollision& b)
        {
            return
                this->collider == b.collider &&
                this->collidee == b.collidee &&
                this->position == b.position &&
                this->distance == b.distance &&
                this->normal == b.normal &&
                this->isIntersecting == b.isIntersecting;
        }

        bool operator !=(const sweepCollision& b)
        {
            return !(*this == b);
        }
    };
}
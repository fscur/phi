#pragma once

#include <phi.h>

#include <core/boxCollider.h>

namespace phi
{
    namespace intersection
    {
        struct singleToSingleTest
        {
            singleToSingleTest() :
                collider(nullptr),
                colliderTransform(nullptr),
                collidee(nullptr),
                collideeTransform(nullptr)
            {
            }

            boxCollider* collider;
            transform* colliderTransform;
            boxCollider* collidee;
            transform* collideeTransform;
        };
    }
}
#pragma once

#include <phi.h>

#include <core/obb.h>

namespace phi
{
    namespace sweep
    {
        struct sweepObbCollision :
            public sweepCollision
        {
            sweepObbCollision() :
                sweepCollision(),
                colliderObb(nullptr)
            {
            }

            const obb* colliderObb;

            bool operator ==(const sweepObbCollision& b)
            {
                return
                    this->colliderObb == b.colliderObb &&
                    sweepCollision::operator==(b);
            }
        };
    }
}
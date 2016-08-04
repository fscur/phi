#pragma once
#include <phi.h>
#include <core\boxCollider.h>

namespace phi
{
    struct rayCastHit
    {
        vec3 position;
        vec3 normal;
        boxCollider* collider;
    };
}

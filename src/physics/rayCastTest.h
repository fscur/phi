#pragma once
#include <phi.h>
#include <core\ray.h>

namespace phi
{
    class rayCastTest
    {
    public:
        phi::ray ray;
        float maxDistance;

    public:
        rayCastTest(phi::ray ray) :
            ray(ray),
            maxDistance(1000.0)
        {
        }

        ~rayCastTest() {}
    };
}
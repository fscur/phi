#pragma once
#include <phi.h>

#include <core/geometry.h>

namespace phi
{
    class iGeometryRepository
    {
    public:
        virtual geometry* getById(const guid& guid) = 0;
    };
}
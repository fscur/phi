#pragma once
#include <phi.h>

#include <core/material.h>

namespace phi
{
    class iMaterialRepository
    {
    public:
        virtual material* getById(const guid& guid) = 0;
    };
}
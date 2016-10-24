#pragma once
#include <phi.h>

#include <core/model.h>

namespace phi
{
    class iModelDataService
    {
    public:
        virtual model* getModelByIndex(int index) = 0;
    };
}
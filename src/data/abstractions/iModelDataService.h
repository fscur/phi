#pragma once
#include <phi.h>

#include <core/model.h>

namespace phi
{
    class iModelDataService
    {
    public:
        virtual model* getModelByIndex(const int index) = 0;
        virtual model* getModelById(const phi::guid& id) = 0;
    };
}
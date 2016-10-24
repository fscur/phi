#pragma once
#include <phi.h>

#include "data/model/modelJson.h"

namespace phi
{
    class iModelRepository
    {
    public:
        virtual modelJson getModelByIndex(int index) = 0;
    };
}
#pragma once
#include <phi.h>

#include "data/model/modelDTO.h"

namespace phi
{
    class iModelRepository
    {
    public:
        virtual modelDTO getModelByIndex(int index) = 0;
    };
}
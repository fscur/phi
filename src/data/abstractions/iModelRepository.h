#pragma once
#include <phi.h>

#include "data/model/modelDTO.h"

namespace phi
{
    class iModelRepository
    {
    public:
        virtual modelDTO getModelByIndex(const int index) = 0;
        virtual modelDTO getModelById(const phi::guid& id) = 0;
    };
}
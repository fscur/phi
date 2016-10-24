#pragma once
#include <phi.h>

#include "dataApi.h"
#include "abstractions/iMaterialRepository.h"

namespace phi
{
    class materialRepository : public iMaterialRepository
    {
    public:
        DATA_API materialRepository();
        DATA_API ~materialRepository();
    };
}
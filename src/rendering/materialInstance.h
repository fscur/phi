#pragma once
#include <phi.h>

#include "materialGpuData.h"

namespace phi
{
    struct materialInstance
    {
        materialInstance(const materialGpuData& materialData, size_t offset) :
            materialData(materialData),
            offset(offset)
        {
        }

        size_t offset;
        materialGpuData materialData;
    };
}

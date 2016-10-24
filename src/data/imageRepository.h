#pragma once
#include <phi.h>

#include "dataApi.h"
#include "abstractions/iImageRepository.h"

namespace phi
{
    class imageRepository : public iImageRepository
    {
    public:
        DATA_API imageRepository();
        DATA_API ~imageRepository();
    };
}
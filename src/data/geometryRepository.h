#pragma once
#include <phi.h>

#include "dataApi.h"
#include "abstractions/iGeometryRepository.h"

namespace phi
{
    class geometryRepository : public iGeometryRepository
    {
    public:
        DATA_API geometryRepository();
        DATA_API ~geometryRepository();
    };
}
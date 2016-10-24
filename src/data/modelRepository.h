#pragma once
#include <phi.h>

#include "dataApi.h"
#include "abstractions/iModelRepository.h"

namespace phi
{
    class modelRepository : public iModelRepository
    {
    public:
        DATA_API modelRepository();
        DATA_API ~modelRepository();

        virtual modelJson getModelByIndex(int index) override;
    };
}
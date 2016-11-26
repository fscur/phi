#pragma once
#include <phi.h>

#include <core/model.h>

#include <data/abstractions/iModelDataService.h>

#include "domainApi.h"

namespace demon
{
    class library
    {
    private:
        phi::iModelDataService* _modelDataService;

    public:
        DOMAIN_API library(phi::iModelDataService* modelDataService);
        DOMAIN_API ~library();

        DOMAIN_API phi::model* getModelByIndex(int index);
    };
}
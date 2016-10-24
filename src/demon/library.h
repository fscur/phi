#pragma once
#include <phi.h>

#include <core/model.h>
#include <core/image.h>
#include <core/geometry.h>
#include <core/material.h>

#include <data/abstractions/iModelDataService.h>

#include <io/path.h>
#include <loader/importResourceException.h>
#include <application/application.h>

namespace demon
{
    class library
    {
    private:
        phi::iModelDataService* _modelDataService;

    private:
        void load();

    public:
        library(phi::iModelDataService* modelDataService);
        ~library();

        phi::model* getModelByIndex(int index);
    };
}
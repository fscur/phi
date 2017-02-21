#include <precompiled.h>
#include "library.h"

#include <io\path.h>

using namespace phi;

namespace demon
{
    library::library(iModelDataService* modelDataService) :
        _modelDataService(modelDataService)
    {
    }

    library::~library()
    {
    }

    model* library::getModelByIndex(const int index)
    {
        return _modelDataService->getModelByIndex(index);
    }

    model* library::getModelById(const phi::guid& id)
    {
        return _modelDataService->getModelById(id);
    }
}
#include <precompiled.h>
#include "library.h"

#include <io\path.h>

using namespace phi;

namespace demon
{
    library::library(iModelDataService* modelDataService) :
        _modelDataService(modelDataService)
    {
        load();
    }

    library::~library()
    {
    }

    void library::load()
    {
        debug(_("Library initialized."));
    }

    model* library::getModelByIndex(int index)
    {
        return _modelDataService->getModelByIndex(index);
    }
}
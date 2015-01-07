#include "globals.h"
#include "core.h"
#include "resourcesRepository.h"
#include "resource.h"

namespace phi
{
    resourcesRepository::resourcesRepository()
    {
    }

    resourcesRepository::~resourcesRepository()
    {
    }

    void resourcesRepository::release()
    {
        for (int i = 0; i < _resources.size(); i++)
            DELETE(_resources[i]);
    }

    void resourcesRepository::addResource(resource* r)
    {
        _resources.push_back(r);
    }
}
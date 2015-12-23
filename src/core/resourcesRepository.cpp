#include <phi/core/resourcesRepository.h>
#include <phi/core/resource.h>
#include <phi/core/globals.h>

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
        for(auto &resource : _resources)
            safeDelete(resource);
        
        //TODO: are those the same ?
        // for (unsigned int i = 0; i < _resources.size(); i++)
        //     safeDelete(_resources[i]);
    }

    void resourcesRepository::addResource(resource* r)
    {
        _resources.push_back(r);
    }
}
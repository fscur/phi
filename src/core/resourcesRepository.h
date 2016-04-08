#pragma once
#include <phi.h>
#include "resource.h"

namespace phi
{
    template<typename T>
    class resourcesRepository
    {
    private:
        vector<resource<T>*> _resources;
        vector<T*> _objects;

    public:
        resourcesRepository()
        {
        }

        ~resourcesRepository()
        {
            for (auto resource : _resources)
                safeDelete(resource);
        }

        void release()
        {
            for(auto resource : _resources)
                safeDelete(resource);
        }

        void addResource(resource<T>* r)
        {
            _resources.push_back(r);
            _objects.push_back((T*)r->getObject());
        }

        vector<resource<T>*> getAllResources()
        {
            return _resources;
        }
        
        vector<T*> getAllObjects()
        {
            return _objects;
        }

        resource<T>* getResource(guid guid)
        {
            for (auto resource : _resources)
            {
                if (resource->getGuid() == guid)
                    return resource;
            }

            return nullptr;
        }
    };
}
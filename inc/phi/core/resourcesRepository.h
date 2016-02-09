#ifndef _PHI_RESOURCES_REPOSITORY_H_
#define _PHI_RESOURCES_REPOSITORY_H_

#include "resource.h"

#include <vector>
#include <string>
#include <sstream>

namespace phi
{
    template<typename T>
    class resourcesRepository
    {
    private:
        std::vector<resource<T>*> _resources;
        std::vector<T*> _objects;

    public:
        resourcesRepository()
        {
        }

        ~resourcesRepository()
        {
        }

        void release()
        {
            for (unsigned int i = 0; i < _resources.size(); i++)
                safeDelete(_resources[i]);
        }

        void addResource(resource<T>* r)
        {
            _resources.push_back(r);
            _objects.push_back((T*)r->getObject());
        }

        std::vector<resource<T>*> getAllResources()
        {
            return _resources;
        }
        
        std::vector<T*> getAllObjects()
        {
            return _objects;
        }

        resource<T>* getResource(GUID guid)
        {
            for (auto resource : _resources)
            {
                if (resource->getGuid() == guid)
                    return resource;
            }

            return nullptr;
        }

        std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string> &elems)
        {
            std::stringstream ss(s);
            std::string item;
            while (std::getline(ss, item, delim))
            {
                if (!item.empty())
                    elems.push_back(item);
            }
            return elems;
        }

        std::vector<std::string> split(const std::string &s, char delim)
        {
            std::vector<std::string> elems;
            split(s, delim, elems);
            return elems;
        }
    };
}

#endif
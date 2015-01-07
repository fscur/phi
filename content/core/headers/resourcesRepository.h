#ifndef _PHI_RESOURCES_REPOSITORY_H_
#define _PHI_RESOURCES_REPOSITORY_H_

#include <vector>
#include "resource.h"

namespace phi
{
    class resourcesRepository
    {
    private:
        std::vector<resource*> _resources;

    public:
        CORE_API resourcesRepository();
        CORE_API ~resourcesRepository();
        CORE_API void release();
        CORE_API void addResource(resource* r);

        template<class T>
        T* getResource(std::string name)
        {
            for (unsigned int i = 0; i < _resources.size(); i++)
            {
                if (_resources[i]->getName() == name)
                    return (T*)_resources[i];
            }

            return nullptr;
        }
    };
}

#endif
#ifndef _PHI_RESOURCES_REPOSITORY_H_
#define _PHI_RESOURCES_REPOSITORY_H_

#include <vector>
#include <string>
#include <sstream>
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

        CORE_API std::vector<resource*> getAllResources()
        {
            return _resources;
        }

        template<class T>
        T* getResource(std::string name)
        {
            for (unsigned int i = 0; i < _resources.size(); i++)
            {
                if (_resources[i]->getFullName() == name)
                    return (T*)_resources[i];
            }

            return nullptr;
        }

        std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems)
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

        std::vector<std::string> getDirectoriesFromPath(std::string path)
        {
            auto directories = std::vector<std::string>();
            auto nodes = split(path, '\\');
            auto nodesSize = nodes.size();

            for (unsigned int i = 0; i < _resources.size(); i++)
            {
                auto res = _resources[i];
                auto resNodes = split(_resources[i]->getFullName(), '\\');
                if (resNodes.size() > nodesSize + 2)
                    continue;

                unsigned int j;
                for (j = 0; j < nodes.size() && resNodes[j] == nodes[j]; j++);
                if (j == nodesSize && std::find(directories.begin(), directories.end(), resNodes[nodesSize]) == directories.end())
                    directories.push_back(resNodes[nodesSize]);
            }

            return directories;
        }

        template<class T>
        std::vector<T*> getResourcesFromPath(std::string path)
        {
            auto resources = std::vector<T*>();
            auto nodes = split(path, '\\');
            auto nodesSize = nodes.size();

            for (unsigned int i = 0; i < _resources.size(); i++)
            {
                auto res = _resources[i];
                auto resNodes = split(_resources[i]->getFullName(), '\\');
                if (resNodes.size() != nodesSize + 1)
                    continue;

                unsigned int j;
                for (j = 0; j < nodes.size() && resNodes[j] == nodes[j]; j++);
                if (j == nodesSize)
                    resources.push_back((T*)res);
            }

            return resources;
        }
    };
}

#endif
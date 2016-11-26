#pragma once
#include <phi.h>

#include <domain/project.h>

namespace demon
{
    class iProjectRepository
    {
    public:
        virtual project* load(phi::string path) = 0;
        virtual void save(project* project, phi::string path) = 0;
    };
}
#ifndef _PHI_RESOURCE_H_
#define _PHI_RESOURCE_H_

#include <string>
#include "core.h"

namespace phi
{
    class resource
    {
    private:
        std::string _name;
        std::string _path;

    public:
        CORE_API resource(std::string name, std::string path)
        {
            _name = name;
            _path = path;
        }
        CORE_API std::string getName() { return _name; }
        CORE_API std::string getPath() { return _path; }
    };
}

#endif
#ifndef _PHI_RESOURCE_H_
#define _PHI_RESOURCE_H_

#include "core.h"

#include <string>

namespace phi
{
    class resource
    {
    protected:
        std::string _name;
        std::string _fullName;
        std::string _path;

    public:
        CORE_API resource(std::string name, std::string path)
        {
            _name = name;
            _path = path;
        }

        CORE_API std::string getName() const { return _name; }
        CORE_API std::string getFullName() const { return _fullName; }
        CORE_API std::string getPath() const { return _path; }
        
        CORE_API void setName(std::string value) { _name = value; }
        CORE_API void setFullName(std::string value) { _fullName = value; }

    };
}

#endif
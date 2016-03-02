#pragma once

#include "core.h"

#include <string>
#include <objbase.h>

namespace phi
{
    template<typename T>
    class resource
    {
    protected:
        GUID _guid;
        std::string _name;
        T* _object;

    public:
        resource(GUID guid, std::string name, T* object) :
            _guid(guid), _name(name), _object(object)
        {
        }

        GUID getGuid() const { return _guid; }
        std::string getName() const { return _name; }
        T* getObject() const { return _object; }
    };
}
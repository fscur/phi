#pragma once
#include <phi.h>
#include "guid.h"

namespace phi
{
    template<typename T>
    class resource
    {
    protected:
        guid _guid;
        string _name;
        T* _object;

    public:
        resource(guid guid, string name, T* object) :
            _guid(guid), _name(name), _object(object)
        {
        }

        ~resource()
        {
            safeDelete(_object);
        }

        guid getGuid() const { return _guid; }
        string getName() const { return _name; }
        T* getOriginalObject() const { return _object; }
		T* getClonedObject() const { return _object->clone(); }
    };
}
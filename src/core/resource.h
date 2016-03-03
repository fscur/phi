#pragma once
#include <phi.h>

namespace phi
{
    template<typename T>
    class resource
    {
    protected:
        GUID _guid;
        string _name;
        T* _object;

    public:
        resource(GUID guid, string name, T* object) :
            _guid(guid), _name(name), _object(object)
        {
        }

        GUID getGuid() const { return _guid; }
        string getName() const { return _name; }
        T* getObject() const { return _object; }
    };
}
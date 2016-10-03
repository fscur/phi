#pragma once
#include <phi.h>
#include "guid.h"

namespace phi
{
    class entity
    {
    protected:
        guid _guid;

    public:
        entity(guid guid) :
            _guid(guid)
        {
        }

        virtual ~entity()
        {
        }

        guid getGuid() const { return _guid; }
    };
}
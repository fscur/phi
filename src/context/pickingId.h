#pragma once
#include <phi.h>
#include "contextApi.h"

#include <core\clickComponent.h>

namespace phi
{
    class pickingId
    {
    private:
        static int _currentId;
        static map<int, clickComponent*> _ids;
    public:
        CONTEXT_API static void setNextId(clickComponent* click);
        CONTEXT_API static clickComponent* get(int id);
        CONTEXT_API static void remove(clickComponent* click);
    };
}
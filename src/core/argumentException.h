#pragma once
#include <phi.h>
#include "coreApi.h"
#include "exception.h"

namespace phi
{
    class argumentException :
        public exception
    {
    public:
        CORE_API argumentException(string message);
    };
}
#pragma once
#include <phi.h>
#include "coreApi.h"
#include "exception.h"

namespace phi
{
    class keyNotFoundException :
        public exception
    {
    public:
        CORE_API keyNotFoundException(string message);
    };
}
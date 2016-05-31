#pragma once
#include <phi.h>
#include "coreApi.h"
#include "exception.h"

namespace phi
{
    class notImplementedException :
        public exception
    {
    public:
        CORE_API notImplementedException();
    };
}
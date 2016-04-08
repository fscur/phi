#pragma once
#include <phi.h>
#include "appsApi.h"
#include <core\exception.h>

namespace phi
{
    class invalidInitializationException :
        public exception
    {
    public:
        APPS_API invalidInitializationException(string message);
    };
}
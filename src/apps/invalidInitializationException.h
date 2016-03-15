#pragma once
#include <phi.h>
#include "apps.h"
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
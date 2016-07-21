#pragma once

#include <phi.h>

#include <core\exception.h>

#include "contextApi.h"

namespace phi
{
    class invalidLayerConfigurationException :
        public exception
    {
    public:
        invalidLayerConfigurationException(string message) :
            exception(message)
        {
        }
    };
}
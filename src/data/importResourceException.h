#pragma once
#include <phi.h>
#include "dataApi.h"

#include <core/exception.h>

namespace phi
{
    class importResourceException :
        public phi::exception
    {
    private:
        string _resourcePath;
    public:
        DATA_API importResourceException(string message);
        DATA_API importResourceException(string message, string resourcePath);
        string getResourcePath() const { return _resourcePath; };
    };
}
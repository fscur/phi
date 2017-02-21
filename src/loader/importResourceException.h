#pragma once
#include <phi.h>
#include "loaderApi.h"

#include <core\exception.h>

namespace phi
{
    class importResourceException :
        public phi::exception
    {
    private:
        string _resourcePath;
    public:
        LOADER_API importResourceException(string message);
        LOADER_API importResourceException(string message, string resourcePath);
        string getResourcePath() const { return _resourcePath; };
    };
}
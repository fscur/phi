#include <precompiled.h>
#include "invalidInitializationException.h"

namespace phi
{
    invalidInitializationException::invalidInitializationException(string message) :
        exception::exception(message)
    {
    }
}

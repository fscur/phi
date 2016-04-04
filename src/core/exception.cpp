#include <precompiled.h>
#include "exception.h"

namespace phi
{
    exception::exception(string message) :
        std::runtime_error(message)
    {
    }
}

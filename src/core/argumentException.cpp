#include <precompiled.h>
#include "argumentException.h"

namespace phi
{
    argumentException::argumentException(string message) :
        exception::exception(message)
    {
    }
}

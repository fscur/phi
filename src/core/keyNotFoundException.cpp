#include <precompiled.h>
#include "keyNotFoundException.h"

namespace phi
{
    keyNotFoundException::keyNotFoundException(string message) :
        exception::exception(message)
    {
    }
}

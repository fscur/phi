#include <precompiled.h>
#include "notImplementedException.h"

namespace phi
{
    notImplementedException::notImplementedException() :
        exception::exception("A not implemented method was called")
    {
    }
}

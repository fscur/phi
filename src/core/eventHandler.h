#ifndef _PHI_EVENTHANDLER_H
#define _PHI_EVENTHANDLER_H

#include "delegate.h"

namespace phi
{
    template <typename T>
    class eventHandler : 
        public delegate<void(T)>
    {   
    };
}

#endif
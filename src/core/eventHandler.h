#pragma once
#include <phi.h>
#include "delegate.h"

namespace phi
{
    template <typename T>
    class eventHandler :
        public delegate<void(T)>
    {
    };
}
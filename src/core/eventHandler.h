#pragma once
#include <precompiled.h>
#include "delegate.h"

namespace phi
{
    template <typename T>
    class eventHandler :
        public delegate<void(T)>
    {
    };
}
#pragma once
#include <phi.h>

namespace phi
{
    class iMouseHoverAnimatable
    {
    public:
        virtual void onMouseEnter() = 0;
        virtual void onMouseLeave() = 0;
    };
}
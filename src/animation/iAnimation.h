#pragma once
#include <phi.h>

namespace phi
{
    class iAnimation
    {
    public:
        virtual void animate() = 0;
        virtual iAnimation* clone() = 0;
        virtual ~iAnimation() {}
    };
}

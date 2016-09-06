#pragma once
#include <phi.h>

#include "iMouseHoverAnimatable.h"

namespace phi
{
    class emptyMouseHoverAnimatable :
        public iMouseHoverAnimatable
    {
    public:
        emptyMouseHoverAnimatable() {};
        ~emptyMouseHoverAnimatable() {};

        virtual void onMouseEnter() override {};
        virtual void onMouseLeave() override {};
    };
}
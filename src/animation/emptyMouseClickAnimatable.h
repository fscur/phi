#pragma once
#include <phi.h>

#include "iMouseClickAnimatable.h"

namespace phi
{
    class emptyMouseClickAnimatable :
        public iMouseClickAnimatable
    {
    public:
        emptyMouseClickAnimatable() {};
        ~emptyMouseClickAnimatable() {};

        virtual void onMouseDown() override {};
        virtual void onMouseUp() override {};

        virtual iMouseClickAnimatable* clone() override { return new emptyMouseClickAnimatable(); }
    };
}
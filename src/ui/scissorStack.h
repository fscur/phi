#pragma once
#include <phi.h>

#include <core\size.h>

#include "scissor.h"

namespace phi
{
    class scissorStack
    {
    private:
        sizef _viewportSize;
        std::vector<scissor> _scissors;
        scissor _currentScissor;

    private:
        void calculateCurrentScissor();

    public:
        scissorStack(sizef viewportSize);

        void pushScissor(scissor s);
        void pushScissor(float x, float y, float width, float height);
        void popScissor();

        void enable();
        void disable();
    };
}
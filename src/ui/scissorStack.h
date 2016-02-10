#ifndef _PHI_SCISSOR_STACK_H_
#define _PHI_SCISSOR_STACK_H_

#include <vector>
#if WIN32
    #include <GL/glew.h>
#else
    #include <OpenGL/gl3.h>
#endif
#include "phi/core/size.h"
#include "phi/ui/scissor.h"

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

#endif
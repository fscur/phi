#pragma once
#include <phi.h>
#include "frameBuffer.h"
namespace phi
{
    class framebuffersManager
    {
    public:
        framebuffersManager() {}
        ~framebuffersManager() {}
        static framebuffer* newFramebuffer() {
            return nullptr;
        }
    };
}
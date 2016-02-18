#pragma once

#include "frameBuffer.h"

namespace phi
{
    class defaultFrameBuffer :
        public frameBuffer
    {
    public:
        RENDERING_API defaultFrameBuffer(sizef size, color clearColor);
        RENDERING_API ~defaultFrameBuffer();
        RENDERING_API void init() override;
        RENDERING_API void clear() override;
    };
}
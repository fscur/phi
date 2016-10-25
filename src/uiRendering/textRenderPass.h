#pragma once
#include <phi.h>

#include <core\resolution.h>

#include <rendering\renderPass.h>

#include "textRenderAdapter.h"
#include <rendering\framebufferAllocator.h>

namespace phi
{
    class textRenderPass
    {
    public:
        textRenderPass() = delete;
        static renderPass * configure(
            textRenderAdapter* renderAdapter,
            const resolution& resolution,
            const string& shadersPath);
    };
}
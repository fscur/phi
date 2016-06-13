#pragma once
#include <phi.h>

#include <core\resolution.h>

#include <rendering\renderPass.h>

namespace phi
{
    class lightingRenderPass
    {
    public:
        lightingRenderPass() = delete;
        static renderPass* configure(
            renderPass* gBufferRenderPass,
            const resolution& resolution,
            const string& shadersPath);
    };
}

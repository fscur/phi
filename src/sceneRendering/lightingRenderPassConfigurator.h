#pragma once
#include <phi.h>

#include <core\resolution.h>

#include <rendering\renderPass.h>

namespace phi
{
    class lightingRenderPassConfigurator
    {
    public:
        lightingRenderPassConfigurator() = delete;
        static renderPass* configureNewLighting(renderPass* gBufferRenderPass, resolution resolution, string& shadersPath);
    };
}

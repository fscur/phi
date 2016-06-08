#pragma once
#include <phi.h>

#include <core\resolution.h>

#include <rendering\renderPass.h>

namespace phi
{
    class lightingRenderPassConfigurator
    {
    private:
        lightingRenderPassConfigurator();

    public:
        static renderPass* configureNewLighting(renderPass* gBufferRenderPass, gl* gl, resolution resolution, string& shadersPath);
    };
}

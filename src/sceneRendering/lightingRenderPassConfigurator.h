#pragma once
#include <phi.h>

#include <rendering\renderPass.h>

namespace phi
{
    class lightingRenderPassConfigurator
    {
    private:
        lightingRenderPassConfigurator();

    public:
        static renderPass* configureNewLighting(renderPass* gBufferRenderPass, gl* gl, float width, float height, string& shadersPath);
    };
}

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
        static renderPass* configureNewLighting();
    };
}

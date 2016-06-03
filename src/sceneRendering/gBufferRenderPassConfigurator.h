#pragma once
#include <phi.h>

#include <rendering\renderPass.h>

namespace phi
{
    class gBufferRenderPassConfigurator
    {
    private:
        gBufferRenderPassConfigurator();
    public:
        static renderPass* configureNewGBuffer();
    };
}
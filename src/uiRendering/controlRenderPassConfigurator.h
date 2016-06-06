#pragma once
#include <phi.h>

#include <rendering\renderPass.h>

#include "controlRendererDescriptor.h"

namespace phi
{
    class controlRenderPassConfigurator
    {
    private:
        controlRenderPassConfigurator();
    public:
        static renderPass* configureNewControlRenderPass(controlRendererDescriptor* rendererDescriptor, gl* gl, float width, float height, string shadersPath);
    };
}
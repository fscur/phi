#pragma once
#include <phi.h>

#include <rendering\renderPass.h>

#include "controlRendererDescriptor.h"

namespace phi
{
    class controlRenderPassConfigurator
    {
    public:
        controlRenderPassConfigurator() = delete;
        static renderPass* configureNewControlRenderPass(controlRendererDescriptor* rendererDescriptor, string shadersPath);
    };
}
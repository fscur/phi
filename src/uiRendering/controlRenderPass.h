#pragma once
#include <phi.h>

#include <rendering\renderPass.h>

#include "controlRendererDescriptor.h"

namespace phi
{
    class controlRenderPass
    {
    public:
        controlRenderPass() = delete;
        static renderPass* configure(controlRendererDescriptor* rendererDescriptor, const string& shadersPath);
    };
}
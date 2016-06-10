#pragma once
#include <phi.h>

#include <rendering\renderPass.h>

#include "glassyControlRendererDescriptor.h"

namespace phi
{
    class glassyControlRenderPass
    {
    public:
        glassyControlRenderPass() = delete;
        static renderPass* configure(glassyControlRendererDescriptor* rendererDescriptor, const string& shadersPath);
    };
}
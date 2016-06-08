#pragma once
#include <phi.h>

#include <rendering\renderPass.h>

#include "glassyControlRendererDescriptor.h"

namespace phi
{
    class glassyControlRenderPassConfigurator
    {
    private:
        glassyControlRenderPassConfigurator();
    public:
        static renderPass* configureNewGlassyControlRenderPass(
            glassyControlRendererDescriptor* rendererDescriptor, 
            gl* gl,
            string shadersPath);
    };
}
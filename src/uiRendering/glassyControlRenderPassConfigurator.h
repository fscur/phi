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
            renderPass* lastRenderPass,
            glassyControlRendererDescriptor* rendererDescriptor, 
            gl* gl, 
            float width, 
            float height, 
            string shadersPath);
    };
}
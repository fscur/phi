#pragma once
#include <phi.h>

#include <rendering\renderPass.h>
#include "meshRendererDescriptor.h"

namespace phi
{
    class gBufferRenderPassConfigurator
    {
    private:
        gBufferRenderPassConfigurator();
    public:
        static renderPass* configureNewGBuffer(meshRendererDescriptor* rendererDescriptor, gl* gl, float width, float height, string shadersPath);
    };
}
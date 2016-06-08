#pragma once
#include <phi.h>

#include <core\resolution.h>

#include <rendering\renderPass.h>

#include "meshRendererDescriptor.h"

namespace phi
{
    class gBufferRenderPassConfigurator
    {
    private:
        gBufferRenderPassConfigurator();
    public:
        static renderPass* configureNewGBuffer(meshRendererDescriptor* rendererDescriptor, gl* gl, resolution resolution, string shadersPath);
    };
}
#pragma once
#include <phi.h>

#include <core\resolution.h>

#include <rendering\renderPass.h>

#include "meshRendererDescriptor.h"

namespace phi
{
    class gBufferRenderPassConfigurator
    {
    public:
        gBufferRenderPassConfigurator() = delete;
        static renderPass* configureNewGBuffer(meshRendererDescriptor* rendererDescriptor, resolution resolution, string shadersPath);
    };
}
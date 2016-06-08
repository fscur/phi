#pragma once
#include <phi.h>

#include <core\resolution.h>

#include <rendering\gl.h>
#include <rendering\renderPass.h>

#include "textRendererDescriptor.h"

namespace phi
{
    class textRenderPassConfigurator
    {
    private:
        textRenderPassConfigurator();
    public:
        static renderPass * configureNewTextRenderPass(textRendererDescriptor* rendererDescriptor, gl * gl, string shadersPath);
    };
}
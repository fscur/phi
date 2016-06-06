#pragma once
#include <phi.h>

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
        static renderPass * configureNewTextRenderPass(textRendererDescriptor* rendererDescriptor, gl * gl, float width, float height, string shadersPath);
    };
}
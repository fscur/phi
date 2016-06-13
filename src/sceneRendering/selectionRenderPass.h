#pragma once
#include <phi.h>

#include <core\resolution.h>
#include <rendering\renderPass.h>

namespace phi
{
    class selectionRenderPass
    {
    public:
        selectionRenderPass() = delete;
        static renderPass* configure(const renderPass* lightingRenderPass, const resolution& resolution, const string& shadersPath);
    };
}

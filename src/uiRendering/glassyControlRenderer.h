#pragma once
#include <phi.h>
#include "uiRenderingApi.h"

#include <rendering\renderPass.h>
#include <rendering\framebufferAllocator.h>

#include "glassyControlRendererDescriptor.h"

namespace phi
{
    class glassyControlRenderer
    {
    private:
        vector<renderPass*> _renderPasses;

    private:
        UI_RENDERING_API glassyControlRenderer(vector<renderPass*>&& renderPasses);

    public:
        UI_RENDERING_API ~glassyControlRenderer();
        UI_RENDERING_API void render();

        UI_RENDERING_API static vector<renderPass*> configure(
            glassyControlRendererDescriptor* renderDescriptor,
            const resolution& resolution,
            const string& resourcesPath,
            framebufferAllocator* framebufferAllocator);
    };
}
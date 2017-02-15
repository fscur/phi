#pragma once
#include <phi.h>
#include "sceneRenderingApi.h"
#include <core\resolution.h>
#include <rendering\renderPass.h>
#include <rendering\framebufferAllocator.h>

#include "translationPlaneGridRenderAdapter.h"

namespace phi
{
    class translationPlaneGridRenderPass :
        public renderPass
    {
    public:
        translationPlaneGridRenderPass(
            translationPlaneGridRenderAdapter* renderAdapter,
            const resolution& resolution,
            const string& shadersPath,
            framebufferAllocator* framebufferAllocator);

        ~translationPlaneGridRenderPass();

        void onBeginRender() override;
        void onEndRender() override;
    };
}

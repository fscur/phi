#pragma once
#include <phi.h>
#include "sceneRenderingApi.h"
#include <core\resolution.h>
#include <rendering\renderPass.h>
#include <rendering\framebufferAllocator.h>

#include "rotationPlaneGridRenderAdapter.h"

namespace phi
{
    class rotationPlaneGridRenderPass :
        public renderPass
    {
    public:
        rotationPlaneGridRenderPass(
            rotationPlaneGridRenderAdapter* renderAdapter,
            const resolution& resolution,
            const string& shadersPath,
            framebufferAllocator* framebufferAllocator);

        ~rotationPlaneGridRenderPass();

        void onBeginRender() override;
        void onEndRender() override;

    private:
        renderTarget* _defaultRenderTarget;
    };
}

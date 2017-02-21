#pragma once
#include <phi.h>
#include "../sceneRenderingApi.h"
#include <core\resolution.h>
#include <rendering\renderPass.h>
#include <rendering\framebufferAllocator.h>

#include "skyBoxRenderAdapter.h"

namespace phi
{
    class skyBoxRenderPass :
        public renderPass
    {
    public:
        skyBoxRenderPass(
            skyBoxRenderAdapter* renderAdapter,
            const resolution& resolution,
            const string& shadersPath,
            framebufferAllocator* framebufferAllocator);

        ~skyBoxRenderPass();

        void onBeginRender() override;
        void onEndRender() override;

    private:
        renderTarget* _defaultRenderTarget;
    };
}

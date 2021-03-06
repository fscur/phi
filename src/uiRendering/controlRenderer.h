#pragma once
#include <phi.h>
#include "uiRenderingApi.h"

#include <core\resolution.h>

#include <rendering\renderPass.h>
#include <rendering\framebufferAllocator.h>
#include "controlRenderAdapter.h"

namespace phi
{
    class controlRenderer
    {
    public:
        UI_RENDERING_API  ~controlRenderer();
        UI_RENDERING_API  void render();

        UI_RENDERING_API static vector<renderPass*> configure(
            controlRenderAdapter* renderAdapter,
            const resolution& resolution,
            const string& resourcesPath,
            framebufferAllocator* framebufferAllocator);

    private:
        UI_RENDERING_API controlRenderer(vector<renderPass*>&& renderPasses);

    private:
        vector<renderPass*> _renderPasses;
    };
}
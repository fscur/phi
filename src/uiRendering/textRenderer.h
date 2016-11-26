#pragma once
#include <phi.h>
#include "uiRenderingApi.h"

#include <rendering\renderPass.h>
#include <rendering\framebufferAllocator.h>
#include "textRenderAdapter.h"

namespace phi
{
    class textRenderer
    {
    private:
        vector<renderPass*> _renderPasses;

    private:
        UI_RENDERING_API textRenderer(vector<renderPass*>&& renderPasses);

    public:
        UI_RENDERING_API  ~textRenderer();
        UI_RENDERING_API  void render();

        UI_RENDERING_API static vector<renderPass*> configure(
            textRenderAdapter* renderAdapter,
            const resolution& resolution,
            const string& resourcesPath);
    };
}
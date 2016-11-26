#pragma once
#include <phi.h>
#include "uiRenderingApi.h"

#include <core\eventArgs.h>
#include <core\resolution.h>
#include <core\node.h>

#include <rendering\renderPass.h>
#include <rendering\framebufferAllocator.h>

#include <uiRendering\textRenderer.h>
#include <uiRendering\textRenderAdapter.h>

namespace phi
{
    class textLayerBehaviour
    {
    private:
        textRenderAdapter* _adapter;
        vector<renderPass*> _renderPasses;

        resolution _resolution;
        string _resourcesPath;

    public:
        UI_RENDERING_API textLayerBehaviour(
            textRenderAdapter* adapter,
            const resolution& resolution,
            const string& resourcesPath);

        UI_RENDERING_API ~textLayerBehaviour();

        UI_RENDERING_API void onNodeAdded(node* node);
        UI_RENDERING_API void onNodeRemoved(node* node);
        UI_RENDERING_API void onNodeTransformChanged(node* node);
        UI_RENDERING_API void onNodeSelectionChanged(node* node);

        vector<renderPass*> getRenderPasses() { return _renderPasses; }
    };
}
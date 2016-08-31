#pragma once
#include <phi.h>
#include "uiRenderingApi.h"

#include <core\eventArgs.h>
#include <core\resolution.h>
#include <core\node.h>

#include <rendering\renderPass.h>
#include <rendering\framebufferAllocator.h>

#include <uiRendering\controlRenderer.h>
#include <uiRendering\controlRenderAdapter.h>

namespace phi
{
    class controlLayerBehaviour
    {
    private:
        controlRenderAdapter* _adapter;
        vector<renderPass*> _renderPasses;

        resolution _resolution;
        string _resourcesPath;

    public:
        UI_RENDERING_API controlLayerBehaviour(
            const resolution& resolution, 
            const string& resourcesPath,
            framebufferAllocator* framebufferAllocator);

        UI_RENDERING_API ~controlLayerBehaviour();

        UI_RENDERING_API void onNodeAdded(node* node);
        UI_RENDERING_API void onNodeRemoved(node* node);
        UI_RENDERING_API void onNodeTransformChanged(node* node);
        UI_RENDERING_API void onNodeSelectionChanged(node* node);

        vector<renderPass*> getRenderPasses() { return _renderPasses; }
    };
}
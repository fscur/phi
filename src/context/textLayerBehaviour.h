#pragma once
#include <phi.h>
#include "contextApi.h"

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
        CONTEXT_API textLayerBehaviour(
            const resolution& resolution, 
            const string& resourcesPath,
            framebufferAllocator* framebufferAllocator);

        CONTEXT_API ~textLayerBehaviour();

        CONTEXT_API void onNodeAdded(node* node);
        CONTEXT_API void onNodeRemoved(node* node);
        CONTEXT_API void onNodeTransformChanged(node* node);
        CONTEXT_API void onNodeSelectionChanged(node* node);

        vector<renderPass*> getRenderPasses() { return _renderPasses; }
    };
}
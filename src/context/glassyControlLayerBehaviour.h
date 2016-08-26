#pragma once
#include <phi.h>
#include "contextApi.h"

#include <core\eventArgs.h>
#include <core\resolution.h>
#include <core\node.h>

#include <rendering\renderPass.h>
#include <rendering\framebufferAllocator.h>

#include <uiRendering\glassyControlRenderer.h>
#include <uiRendering\controlRenderAdapter.h>

namespace phi
{
    class glassyControlLayerBehaviour
    {
    private:
        controlRenderAdapter* _adapter;
        vector<renderPass*> _renderPasses;

        resolution _resolution;
        string _resourcesPath;

    private:
        void onControlColorChanged(control * control);

    public:
        CONTEXT_API glassyControlLayerBehaviour(
            const resolution& resolution, 
            const string& resourcesPath,
            framebufferAllocator* framebufferAllocator);

        CONTEXT_API ~glassyControlLayerBehaviour();

        CONTEXT_API void onNodeAdded(node* node);
        CONTEXT_API void onNodeRemoved(node* node);
        CONTEXT_API void onNodeTransformChanged(node* node);
        CONTEXT_API void onNodeSelectionChanged(node* node);

        vector<renderPass*> getRenderPasses() { return _renderPasses; }
    };
}
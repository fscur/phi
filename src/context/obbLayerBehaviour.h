#pragma once
#include <phi.h>
#include "contextApi.h"

#include <core\eventArgs.h>
#include <core\resolution.h>
#include <core\node.h>

#include <rendering\renderPass.h>
#include <rendering\framebufferAllocator.h>

#include <debugRendering\obbRenderAdapter.h>

namespace phi
{
    class obbLayerBehaviour
    {
    private:
        obbRenderAdapter* _adapter;
        vector<renderPass*> _renderPasses;

        vector<node*> _selectedObjects;

        resolution _resolution;
        string _resourcesPath;

    private:
        bool shouldShowObbOf(node* node);

    public:
        CONTEXT_API obbLayerBehaviour(const resolution& resolution, const string& resourcesPath, framebufferAllocator* framebufferAllocator);
        CONTEXT_API ~obbLayerBehaviour();

        CONTEXT_API void onNodeAdded(node* node);
        CONTEXT_API void onNodeRemoved(node* node);
        CONTEXT_API void onNodeTransformChanged(node* node);
        CONTEXT_API void onNodeObbChanged(node* node);

        vector<renderPass*> getRenderPasses() { return _renderPasses; }
    };
}
#pragma once
#include <phi.h>
#include "debugRenderingApi.h"

#include <core\eventArgs.h>
#include <core\resolution.h>
#include <core\node.h>

#include <rendering\renderPass.h>
#include <rendering\framebufferAllocator.h>

#include "boxColliderRenderAdapter.h"

namespace phi
{
    class boxColliderLayerBehaviour
    {
    private:
        boxColliderRenderAdapter* _adapter;
        vector<renderPass*> _renderPasses;

        vector<node*> _selectedObjects;

        resolution _resolution;
        string _resourcesPath;

    public:
        DEBUG_RENDERING_API boxColliderLayerBehaviour(const resolution& resolution, const string& resourcesPath, framebufferAllocator* framebufferAllocator);
        DEBUG_RENDERING_API ~boxColliderLayerBehaviour();

        DEBUG_RENDERING_API void onNodeAdded(node* node);
        DEBUG_RENDERING_API void onNodeRemoved(node* node);
        DEBUG_RENDERING_API void onNodeTransformChanged(node* node);
        DEBUG_RENDERING_API void onNodeComponentAdded(node* node, component* component);
        DEBUG_RENDERING_API void onNodeComponentRemoved(node* node, component* component);

        vector<renderPass*> getRenderPasses() { return _renderPasses; }
    };
}
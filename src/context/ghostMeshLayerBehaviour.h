#pragma once
#include <phi.h>
#include "contextApi.h"

#include <core\eventArgs.h>
#include <core\resolution.h>
#include <core\node.h>

#include <rendering\renderPass.h>
#include <rendering\framebufferAllocator.h>

#include <sceneRendering\ghostMeshRenderAdapter.h>

namespace phi
{
    class ghostMeshLayerBehaviour
    {
    private:
        ghostMeshRenderAdapter* _adapter;
        vector<renderPass*> _renderPasses;

        vector<node*> _selectedObjects;

        resolution _resolution;
        string _resourcesPath;

    public:
        CONTEXT_API ghostMeshLayerBehaviour(const resolution& resolution, const string& resourcesPath, framebufferAllocator* framebufferAllocator);
        CONTEXT_API ~ghostMeshLayerBehaviour();

        CONTEXT_API void onNodeAdded(node* node);
        CONTEXT_API void onNodeRemoved(node* node);
        CONTEXT_API void onNodeTransformChanged(node* node);

        vector<renderPass*> getRenderPasses() { return _renderPasses; }
        vector<node*> getSelectedObjects() { return _selectedObjects; }
    };
}
#pragma once
#include <phi.h>
#include "../sceneRenderingApi.h"

#include <core\eventArgs.h>
#include <core\resolution.h>
#include <core\node.h>

#include <rendering\renderPass.h>
#include <rendering\framebufferAllocator.h>

#include "skyBoxRenderAdapter.h"

namespace phi
{
    class skyBoxLayerBehaviour
    {
    private:
        skyBoxRenderAdapter* _adapter;
        vector<renderPass*> _renderPasses;

        vector<node*> _selectedObjects;

        resolution _resolution;
        string _resourcesPath;

    public:
        SCENE_RENDERING_API skyBoxLayerBehaviour(
            const resolution& resolution, 
            const string& resourcesPath, 
            framebufferAllocator* framebufferAllocator);

        SCENE_RENDERING_API ~skyBoxLayerBehaviour();

        SCENE_RENDERING_API void onNodeAdded(node* node);
        SCENE_RENDERING_API void onNodeRemoved(node* node);
        SCENE_RENDERING_API void onNodeTransformChanged(node* node);

        vector<renderPass*> getRenderPasses() { return _renderPasses; }
    };
}
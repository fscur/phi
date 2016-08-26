#pragma once
#include <phi.h>
#include "sceneRenderingApi.h"

#include <core\eventArgs.h>
#include <core\resolution.h>
#include <core\node.h>

#include <rendering\renderPass.h>
#include <rendering\framebufferAllocator.h>

#include "planeGridRenderAdapter.h"

namespace phi
{
    class planeGridLayerBehaviour
    {
    private:
        planeGridRenderAdapter* _adapter;
        vector<renderPass*> _renderPasses;

        vector<node*> _selectedObjects;

        resolution _resolution;
        string _resourcesPath;

    public:
        SCENE_RENDERING_API planeGridLayerBehaviour(
            const resolution& resolution, 
            const string& resourcesPath, 
            framebufferAllocator* framebufferAllocator);

        SCENE_RENDERING_API ~planeGridLayerBehaviour();

        SCENE_RENDERING_API void onNodeAdded(node* node);
        SCENE_RENDERING_API void onNodeRemoved(node* node);
        SCENE_RENDERING_API void onNodeTransformChanged(node* node);
        SCENE_RENDERING_API void onNodeSelectionChanged(node* node);

        vector<renderPass*> getRenderPasses() { return _renderPasses; }
    };
}
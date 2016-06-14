#pragma once
#include <phi.h>
#include "contextApi.h"

#include <core\eventArgs.h>
#include <core\resolution.h>
#include <core\node.h>

#include <rendering\renderPass.h>

#include <uiRendering\controlRenderer.h>
#include <uiRendering\controlRendererDescriptor.h>

namespace phi
{
    class controlLayerBehaviour
    {
    private:
        controlRendererDescriptor* _descriptor;
        vector<renderPass*> _renderPasses;

        resolution _resolution;
        string _resourcesPath;

    public:
        CONTEXT_API controlLayerBehaviour(const resolution& resolution, const string& resourcesPath);
        CONTEXT_API ~controlLayerBehaviour();

        CONTEXT_API void onNodeAdded(node* node);
        CONTEXT_API void onNodeRemoved(node* node);
        CONTEXT_API void onNodeTransformChanged(node* node);
        CONTEXT_API void onNodeSelectionChanged(node* node);

        vector<renderPass*> getRenderPasses() { return _renderPasses; }
    };
}
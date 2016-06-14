#pragma once
#include <phi.h>
#include "contextApi.h"

#include <core\eventArgs.h>
#include <core\resolution.h>
#include <core\node.h>

#include <rendering\renderPass.h>

#include <uiRendering\glassyControlRenderer.h>
#include <uiRendering\glassyControlRendererDescriptor.h>

namespace phi
{
    class glassyControlLayerBehaviour
    {
    private:
        glassyControlRendererDescriptor* _descriptor;
        vector<renderPass*> _renderPasses;

        resolution _resolution;
        string _resourcesPath;

    public:
        CONTEXT_API glassyControlLayerBehaviour(const resolution& resolution, const string& resourcesPath);
        CONTEXT_API ~glassyControlLayerBehaviour();

        CONTEXT_API void onNodeAdded(node* node);
        CONTEXT_API void onNodeRemoved(node* node);
        CONTEXT_API void onNodeTransformChanged(node* node);
        CONTEXT_API void onNodeSelectionChanged(node* node);

        vector<renderPass*> getRenderPasses() { return _renderPasses; }
    };
}
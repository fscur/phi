#pragma once

#include <phi.h>

#include <core/node.h>

#include "../layersApi.h"

namespace phi
{
    class selectionLayerBehaviour
    {
    private:
        vector<node*> _selectedNodes;

    public:
        eventHandler<selectionLayerBehaviour*> selectedNodesChanged;

    public:
        LAYERS_API selectionLayerBehaviour();

        LAYERS_API const vector<node*>* getSelectedNodes() const { return &_selectedNodes; }

        LAYERS_API void onNodeSelectionChanged(node* node);
    };
}
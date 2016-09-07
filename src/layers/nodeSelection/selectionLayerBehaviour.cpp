#include <precompiled.h>

#include "selectionLayerBehaviour.h"

namespace phi
{
    selectionLayerBehaviour::selectionLayerBehaviour() :
        _selectedNodes(vector<node*>()),
        selectedNodesChanged(eventHandler<selectionLayerBehaviour*>())
    {
    }

    void selectionLayerBehaviour::onNodeSelectionChanged(node* node)
    {
        if (node->isSelected())
        {
            _selectedNodes.push_back(node);
            selectedNodesChanged.raise(this);
        }
        else
        {
            if (phi::removeIfContains(_selectedNodes, node))
                selectedNodesChanged.raise(this);
        }
    }
}
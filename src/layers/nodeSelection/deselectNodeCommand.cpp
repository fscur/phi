#include <precompiled.h>
#include "deselectNodeCommand.h"

namespace phi
{
    deselectNodeCommand::deselectNodeCommand(const vector<node*>& nodesToDeselect) :
        _nodesToDeselect(nodesToDeselect)
    {
    }

    deselectNodeCommand::~deselectNodeCommand()
    {
    }

    void deselectNodeCommand::execute()
    {
        for (auto node : _nodesToDeselect)
        {
            if (node->isSelected())
            {
                node->deselect();
                _deselectedNodes.push_back(node);
            }
        }
    }

    void deselectNodeCommand::executeUndo()
    {
        for (auto node : _deselectedNodes)
            node->select();
    }
}
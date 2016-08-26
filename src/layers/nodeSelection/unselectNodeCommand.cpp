#include <precompiled.h>
#include "unselectNodeCommand.h"

namespace phi
{
    unselectNodeCommand::unselectNodeCommand(const vector<node*>& nodesToUnselect) :
        _nodesToUnselect(nodesToUnselect)
    {
    }

    unselectNodeCommand::~unselectNodeCommand()
    {
    }

    void unselectNodeCommand::execute()
    {
        for (auto node : _nodesToUnselect)
        {
            if (node->isSelected())
            {
                node->deselect();
                _unselectedNodes.push_back(node);
            }
        }
    }

    void unselectNodeCommand::executeUndo()
    {
        for (auto node : _unselectedNodes)
            node->select();
    }
}
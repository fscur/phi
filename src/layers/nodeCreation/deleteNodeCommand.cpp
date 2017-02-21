#include <precompiled.h>
#include "deleteNodeCommand.h"

namespace phi
{
    deleteNodeCommand::deleteNodeCommand(const vector<node*>& nodesToDelete) :
        _nodesToDelete(nodesToDelete)
    {
    }

    deleteNodeCommand::~deleteNodeCommand()
    {
    }

    void deleteNodeCommand::execute()
    {
        for (auto node : _nodesToDelete)
        {
            auto parent = node->getParent();
            parent->removeChild(node);

            _nodesParents[node] = parent;
        }
    }

    void deleteNodeCommand::executeUndo()
    {
        for (auto pair : _nodesParents)
        {
            pair.second->addChild(pair.first);
        }

        _nodesParents.clear();
    }
}
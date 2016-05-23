#include <precompiled.h>
#include "deleteSceneObjectCommand.h"

namespace phi
{
    deleteSceneObjectCommand::deleteSceneObjectCommand(const vector<node*>& nodesToDelete) :
        _nodesToDelete(nodesToDelete)
    {
    }

    deleteSceneObjectCommand::~deleteSceneObjectCommand()
    {
    }

    void deleteSceneObjectCommand::execute()
    {
        debug("delete");
        for (auto node : _nodesToDelete)
        {
            auto parent = node->getParent();
            parent->removeChild(node);

            _nodesParents[node] = parent;
        }
    }

    void deleteSceneObjectCommand::executeUndo()
    {
        debug("undelete");
        for (auto pair : _nodesParents)
        {
            pair.second->addChild(pair.first);
        }

        _nodesParents.clear();
    }
}
#include <precompiled.h>
#include "unselectSceneObjectCommand.h"

namespace phi
{
    unselectSceneObjectCommand::unselectSceneObjectCommand(const vector<node*>& nodesToUnselect) :
        _nodesToUnselect(nodesToUnselect)
    {
    }

    unselectSceneObjectCommand::~unselectSceneObjectCommand()
    {
    }

    void unselectSceneObjectCommand::execute()
    {
        for (auto node : _nodesToUnselect)
        {
            if (node->getIsSelected())
            {
                node->setIsSelected(false);
                _unselectedNodes.push_back(node);
            }
        }
    }

    void unselectSceneObjectCommand::executeUndo()
    {
        for (auto node : _unselectedNodes)
        {
            node->setIsSelected(true);
        }
    }
}

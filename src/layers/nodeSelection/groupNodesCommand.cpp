#include <precompiled.h>
#include "groupNodesCommand.h"

namespace phi
{
    groupNodesCommand::groupNodesCommand(vector<node*>& nodesToGroup) :
        _nodesToGroup(nodesToGroup)
    {
    }

    groupNodesCommand::~groupNodesCommand()
    {
    }

    void groupNodesCommand::execute()
    {
        if (_nodesToGroup.size() < 2)
            return;

        _group = new node("group");

        auto parent = _nodesToGroup.at(0)->getParent();

        for (auto node : _nodesToGroup)
        {
            parent->removeChild(node);
            _group->addChild(node);
        }

        parent->addChild(_group);
    }

    void groupNodesCommand::executeUndo()
    {
        auto parent = _group->getParent();
        parent->removeChild(_group);

        for (auto child : _nodesToGroup)
        {
            _group->removeChild(child);
            parent->addChild(child);
        }

        safeDelete(_group);
    }
}
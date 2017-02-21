#include <precompiled.h>
#include "multiselectNodeCommand.h"

namespace phi
{
    multiselectNodeCommand::multiselectNodeCommand(node* nodeToSelect) :
        _nodeToSelect(nodeToSelect)
    {
    }

    multiselectNodeCommand ::~multiselectNodeCommand()
    {
    }

    node* multiselectNodeCommand::getSonOfRoot(node* node)
    {
        auto sonOfRoot = node;

        auto parent = _nodeToSelect->getParent();
        while (parent && parent->getParent())
        {
            sonOfRoot = parent;
            parent = parent->getParent();
        }

        return sonOfRoot;
    }

    void multiselectNodeCommand::execute()
    {
        _nodeToSelect = getSonOfRoot(_nodeToSelect);

        _nodeToSelect->traverse([](node* node)
        {
            node->select();
        });
    }

    void multiselectNodeCommand::executeUndo()
    {
        _nodeToSelect->traverse([](node* node)
        {
            node->deselect();
        });
    }
}

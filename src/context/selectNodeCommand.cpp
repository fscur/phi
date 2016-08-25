#include <precompiled.h>

#include "selectNodeCommand.h"

namespace phi
{
    selectNodeCommand::selectNodeCommand(node* nodeToSelect) :
        _nodeToSelect(nodeToSelect)
    {
    }

    selectNodeCommand::~selectNodeCommand()
    {
    }

    void selectNodeCommand::execute()
    {
        _nodeToSelect->select();
    }

    void selectNodeCommand::executeUndo()
    {
        _nodeToSelect->deselect();
    }
}